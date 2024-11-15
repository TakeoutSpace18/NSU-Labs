#define _GNU_SOURCE
#include "server.h"

#include <ev.h>

#include "c.h"
#include "dns.h"
#include "coro.h"
#include "list.h"
#include "socket.h"
#include "log.h"

#define BACKLOG 128

void server_coroutine_main(void *arg);
static void on_accept_cb(EV_P_ struct ev_io *w, int revents);
static void on_client_wakeup_cb(EV_P_ struct ev_io *w, int revents);
static void on_client_drop_cb(EV_P_ struct ev_async *w, int revents);

client_t *g_running_client = NULL;

int server_create_listening_socket(uint16_t port, int *sockfd) {
    struct sockaddr_in sa;
    *sockfd = -1;

    if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        log_error("socket() failed: %s", strerror(errno));
        goto fail;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if (bind(*sockfd, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
        log_error("bind() failed: %s", strerror(errno));
        goto fail;
    }

    if (listen(*sockfd, BACKLOG) == -1) {
        log_error("listen() failed: %s", strerror(errno));
        goto fail;
    }

    return 0;

fail:
    if (*sockfd >= 0)
        close(*sockfd);
    return -1;
}

int server_init(server_t *s, uint16_t port, client_routine_t routine) {
    int sockfd;

    s->client_routine = routine;
    list_init(&s->clients);

    if (server_create_listening_socket(port, &sockfd) == -1) {
        goto fail;
    }

    coro_create(&s->context, NULL, NULL, NULL, 0);

    s->loop = ev_default_loop(0);
    ev_io_init(&s->accept_watcher, on_accept_cb, sockfd, EV_READ);
    ev_io_start(s->loop, &s->accept_watcher);

    dns_init(&s->dns, s->loop);

    return 0;

fail:
    if (sockfd >= 0)
        close(sockfd);

    return -1;
}

void server_run(server_t *s) {
    ev_run(s->loop, 0);
}

void server_finalize(server_t *s)
{
    dns_finalize(&s->dns);
}

static client_t *server_client_add(server_t *server, int sockfd, const char *descr)
{
    client_t *c = malloc(sizeof(*c));
    if (!c) {
        log_error("malloc() failed: %s", strerror(errno));
        goto fail;
    }

    c->server_p = server;
    c->description = strdup(descr);

    if (!coro_stack_alloc(&c->stack, 0)) {
        log_error("Failed to allocate client stack");
        goto fail;
    }

    coro_create(&c->context, (coro_func) server->client_routine, c,
                c->stack.sptr, c->stack.ssze);

    ev_io_init(&c->io_watcher, on_client_wakeup_cb, sockfd, EV_READ);
    ev_io_start(server->loop, &c->io_watcher);

    ev_async_init(&c->drop_watcher, on_client_drop_cb);
    ev_async_start(server->loop, &c->drop_watcher);

    list_push(&server->clients, &c->link);

    log_info("[%s] New client connected", descr);

    return c;

fail:
    if (c)
        coro_stack_free(&c->stack);
    free(c);
    return NULL;
}

void switch_to_client(client_t *client)
{
    assert(g_running_client == NULL);

    coro_context *client_ctx = &client->context;
    coro_context *server_ctx = &client->server_p->context;

    g_running_client = client;

    log_trace("context switch: server --> client(%s)", client->description);
    coro_transfer(server_ctx, client_ctx);
}

void switch_to_server(client_t *client)
{
    assert(g_running_client != NULL);

    coro_context *client_ctx = &client->context;
    coro_context *server_ctx = &client->server_p->context;

    g_running_client = NULL;

    log_trace("context switch: client(%s) --> server", client->description);
    coro_transfer(client_ctx, server_ctx);
}

void client_yield(void)
{
    client_t *c = g_running_client;

    switch_to_server(c);
}

void attribute_noreturn() client_drop(void)
{
    client_t *c = g_running_client;

    /* releasing client resources happens in server coroutine, 
     * because we can't destroy stack of currently running client coroutine
     * from itself */
    ev_async_send(c->server_p->loop, &c->drop_watcher);

    coro_context *client_context = &c->context;
    coro_context *server_context = &c->server_p->context;

    /* loop in case client coroutine will wake up again, but it shouldn't */
    for (;;) {
        switch_to_server(c);
    }
}

ssize_t client_recv(void *buf, size_t n, int flags)
{
    if (n == 0) assert(0);
    ssize_t ret;
    for (;;) {
        ret = recv(client_sockfd(), buf, n, flags);

        if (ret == -1 && errno == EAGAIN) {
            client_yield();
        }
        else
            return ret;

    }
}

ssize_t client_send(void *buf, size_t n, int flags)
{
    ssize_t ret;
    for (;;) {
        ret = send(client_sockfd(), buf, n, flags);
        if (ret == -1 && errno == EAGAIN)
            client_yield();
        else
            return ret;
    }
}

ssize_t client_recv_buf(void *buf, size_t size)
{
    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = client_recv((char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        recv_size += ret;
    }

    return size;
}

ssize_t client_send_buf(void *buf, size_t size)
{
    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = client_send((char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        sent_size += ret;
    }

    return size;
}

static void on_accept_cb(EV_P_ struct ev_io *w, int revents)
{
    server_t *server = (server_t *) w;

    int sockfd;
    struct sockaddr_in sa;
    socklen_t sa_len;

    if (!read_available(revents)) {
        log_warn("accept watcher called without available read");
        return;
    }

    if ((sockfd = accept4(w->fd, &sa, &sa_len, SOCK_NONBLOCK)) < 0) {
        if (errno == EWOULDBLOCK) {
            return;
        }

        log_error("accept4() failed: %s", strerror(errno));
        return;
    }

    char descr_buf[SOCKADDR2STR_MAX_BUFSIZE];
    sockaddr2str((struct sockaddr *) &sa, sa_len, descr_buf);

    client_t *new_client = server_client_add(server, sockfd, descr_buf);
    if (!new_client) {
        return;
    }

    switch_to_client(new_client);
}

static void on_client_wakeup_cb(EV_P_ ev_io *w, int revents)
{
    client_t *client = (client_t *) io_w_2_client(w);
    switch_to_client(client);
}

static void on_client_drop_cb(EV_P_ ev_async *w, int revents)
{
    client_t *client = (client_t *) drop_w_2_client(w);

    log_info("[%s] Closing connection...", client->description);
    
    ev_io_stop(client->server_p->loop, &client->io_watcher);
    close(client->io_watcher.fd);
    list_unlink(&client->link);
    coro_stack_free(&client->stack);
    free(client->description);
    free(client);
}
