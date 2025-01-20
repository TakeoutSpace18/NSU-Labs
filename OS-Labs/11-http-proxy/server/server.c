#include "dynarray.h"
#define _GNU_SOURCE
#include "server.h"

#include "dns.h"
#include "list.h"
#include "log.h"
#include "socket.h"
#include "client_context.h"
#include "worker_thread.h"

#define BACKLOG 128

static void on_accept_cb(EV_P_ struct ev_io *w, int revents);

static int create_listening_socket(uint16_t port, int *sockfd) {
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

static inline void ignore_sigpipe(void)
{
    struct sigaction sa = { 0 };
    sa.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &sa, NULL);
}

static int create_workers(server_t *s, size_t nr_workers)
{
    dynarray_create(&s->workers, sizeof(worker_thread_t));
    if (dynarray_reserve(&s->workers, nr_workers) != OK) {
        log_error("out of memory");
        return ERROR;
    }

    size_t created;
    for (size_t i = 0; i < nr_workers; ++i) {
        worker_thread_t *worker = dynarray_emplace_back(&s->workers);
        if (worker_thread_create(worker, i) != OK) {
            created = i;
            goto fail;
        }
    }

    return OK;

fail:
    for (size_t i = 0; i < created; i++) {
        worker_thread_destroy(dynarray_at(&s->workers, i));
    }

    dynarray_destroy(&s->workers);
    return ERROR;
}

int server_create(server_t *s, uint16_t port, client_routine_t routine,
                  void *routine_arg, size_t nr_workers)
{
    /* to avoid terminating whole server on SIGPIPE caused by some send */
    ignore_sigpipe();

    s->client_routine = routine;
    s->client_routine_arg = routine_arg;

    int sockfd;
    if (create_listening_socket(port, &sockfd) == -1) {
        goto fail;
    }

    if (dns_init() != OK) {
        goto fail;
    }

    if (create_workers(s, nr_workers) != OK) {
        goto fail;
    }

    list_init(&s->clients);
    s->nr_clients = 0;

    s->loop = ev_default_loop(0);
    ev_io_init(&s->accept_watcher, on_accept_cb, sockfd, EV_READ);
    ev_io_start(s->loop, &s->accept_watcher);

    return 0;

fail:
    if (sockfd >= 0)
        close(sockfd);

    return -1;
}

void server_destroy(server_t *s)
{
    dns_finalize();
}

void server_run(server_t *s) {
    ev_run(s->loop, 0);
}

static worker_thread_t *choose_worker(dynarray_t *workers)
{
    static size_t last_chosen = 0;

    size_t i = (last_chosen + 1) % dynarray_size(workers);
    last_chosen = i;
    return dynarray_at(workers, i);
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

        log_error("Falied to accept connection, accept4() : %s", strerror(errno));
        return;
    }

    char descr_buf[SOCKADDR2STR_MAX_BUFSIZE];
    sockaddr2str((struct sockaddr *) &sa, sa_len, descr_buf);

    worker_thread_t *worker = choose_worker(&server->workers);

    client_context_t *new_client = malloc(sizeof(*new_client));
    if (!new_client) {
        log_error("Failed to accept connection: out of memory");
        return;
    }

    int err = client_context_create(new_client, server->client_routine,
                                    server->client_routine_arg,
                                    sockfd, descr_buf, worker);
    if (err != OK) {
        free(new_client);
        log_error("Failed to accept connection: can't create client context");
        return;
    }

    list_push_back(&server->clients, &new_client->link);
    server->nr_clients++;

    async_client_wakeup(new_client);
}
