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

#define IN_CLIENT_CONTEXT (g_running_client != NULL)

static int server_create_listening_socket(uint16_t port, int *sockfd) {
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

static void add_io_watcher(client_t *c, int fd)
{
    if (c->nr_io_watchers == MAX_CLIENT_IO_WATCHERS) {
        if (IN_CLIENT_CONTEXT) {
            client_log_fatal("Per client io watchers limit exceeded");
            client_drop();
        } else {
            log_fatal("Per client io watchers limit exceeded");
            /* this situation should never happen */
            abort();
        }
    }

    size_t i = c->nr_io_watchers;
    c->io_watchers[i].data = c;
    ev_io_init(&c->io_watchers[i], on_client_wakeup_cb, fd, EV_READ | EV_WRITE);
    ev_io_start(c->server_p->loop, &c->io_watchers[i]);

    c->nr_io_watchers++;
}

/* returns 0 on success, -1 on watcher not found */
static int remove_io_watcher(client_t *c, int fd)
{
    size_t i = 0;
    while (i < c->nr_io_watchers && c->io_watchers[i].fd != fd)
        i++;

    if (i == c->nr_io_watchers) {
        return -1;
    }

    ev_io_stop(c->server_p->loop, &c->io_watchers[i]);

    memmove(&c->io_watchers[i], &c->io_watchers[i + 1],
            (c->nr_io_watchers - i - 1) * sizeof(c->io_watchers[0]));

    c->nr_io_watchers--;

    return 0;
}

static inline void ignore_sigpipe(void)
{
    struct sigaction sa = { 0 };
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);
}

int server_init(server_t *s, uint16_t port, client_routine_t routine) {
    int sockfd;

    s->client_routine = routine;
    list_init(&s->clients);

    /* to avoid terminating whole server on SIGPIPE caused by some send */
    ignore_sigpipe();

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


    c->nr_io_watchers = 0;
    add_io_watcher(c, sockfd);

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

void server_switch_to_client(client_t *client)
{
    assert(g_running_client == NULL);

    coro_context *client_ctx = &client->context;
    coro_context *server_ctx = &client->server_p->context;

    g_running_client = client;

    log_trace("context switch: server --> client(%s)", client->description);
    coro_transfer(server_ctx, client_ctx);
}

static void server_switch_to_server(client_t *client)
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

    server_switch_to_server(c);
}

void attribute_noreturn() client_drop(void)
{
    client_t *c = g_running_client;

    /* releasing client resources happens in server coroutine, 
     * because we can't destroy stack of currently running client coroutine
     * from itself */
    ev_async_send(c->server_p->loop, &c->drop_watcher);

    /* loop in case client coroutine will wake up again, but it shouldn't */
    for (;;) {
        server_switch_to_server(c);
    }
}

ssize_t client_recv(int fd, void *buf, size_t n, int flags)
{
    ssize_t ret;
    for (;;) {
        ret = recv(fd, buf, n, flags);
        if (ret == -1 && errno == EAGAIN)
            client_yield();
        else
            return ret;
    }
}

ssize_t client_send(int fd, void *buf, size_t n, int flags)
{
    ssize_t ret;
    for (;;) {
        ret = send(fd, buf, n, flags);
        if (ret == -1 && errno == EAGAIN)
            client_yield();
        else
            return ret;
    }
}

ssize_t client_recv_buf(int fd, void *buf, size_t size)
{
    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = client_recv(fd, (char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        recv_size += ret;
    }

    return size;
}

ssize_t client_send_buf(int fd, void *buf, size_t size)
{
    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = client_send(fd, (char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        sent_size += ret;
    }

    return size;
}

ssize_t client_recv_nonblock(int fd, void *buf, size_t n, int flags)
{
    ssize_t ret;

    ret = recv(fd, buf, n, flags);
    if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return NODATA;
    else
        return ret;
}

ssize_t client_send_nonblock(int fd, void *buf, size_t n, int flags)
{
    ssize_t ret;

    ret = send(fd, buf, n, flags);
    if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return NODATA;
    else
        return ret;
}

void client_watch_fd(int fd)
{
    client_t *c = g_running_client;

    if (set_nonblock(fd) != 0) {
        client_log_error("client_watch_fd(): Failed to set fd to nonblocking mode, %s", 
                         strerror(errno));
        client_drop();
    }

    add_io_watcher(c, fd);
}

void client_unwatch_fd(int fd)
{
    client_t *c = g_running_client;
    int ret = remove_io_watcher(c, fd);
    if (ret != 0)
        client_log_warn("client_unwatch_fd(): watcher not found");
}

static void on_accept_cb(EV_P_ struct ev_io *w, int revents)
{
    server_t *server = (server_t *) accept_w_2_server(w);

    int sockfd;
    struct sockaddr_in sa;
    socklen_t sa_len = sizeof(sa);

    if (!read_available(revents)) {
        log_warn("accept watcher called without available read");
        return;
    }

    if ((sockfd = accept4(w->fd, &sa, &sa_len, SOCK_NONBLOCK)) < 0) {
        if (errno == EWOULDBLOCK) {
            return;
        }

        log_info("sa_len: %i", sa_len);
        log_error("accept4() failed: %s", strerror(errno));
        return;
    }

    char descr_buf[SOCKADDR2STR_MAX_BUFSIZE];
    sockaddr2str((struct sockaddr *) &sa, sa_len, descr_buf);

    client_t *new_client = server_client_add(server, sockfd, descr_buf);
    if (!new_client) {
        return;
    }

    server_switch_to_client(new_client);
}


static void on_client_wakeup_cb(EV_P_ ev_io *w, int revents)
{
    client_t *client = (client_t *) w->data;
    server_switch_to_client(client);
}

static void on_client_drop_cb(EV_P_ ev_async *w, int revents)
{
    client_t *client = (client_t *) drop_w_2_client(w);
    log_info("[%s] Closing connection...", client->description);
    
    for (size_t i = 0; i < client->nr_io_watchers; ++i) {
        ev_io_stop(client->server_p->loop, &client->io_watchers[i]);
    }

    ev_async_stop(client->server_p->loop, &client->drop_watcher);

    int client_sfd = client->io_watchers[0].fd;
    close(client_sfd);

    list_unlink(&client->link);
    coro_stack_free(&client->stack);
    free(client->description);
    free(client);
}
