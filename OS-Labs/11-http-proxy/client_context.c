#define _GNU_SOURCE
#include "client_context.h"

#include <ev.h>

#include "c.h"
#include "dns.h"
#include "log.h"
#include "list.h"
#include "socket.h"
#include "coroutine.h"
#include "worker_thread.h"

static void on_client_async_wakeup_cb(EV_P_ struct ev_async *w, int revents);
static void on_client_io_available_cb(EV_P_ struct ev_io *w, int revents);
static void on_client_drop_cb(EV_P_ struct ev_async *w, int revents);

client_context_t thread_local *g_running_client = NULL;

#define IN_CLIENT_CONTEXT (g_running_client != NULL)

static fdwatcher_t add_io_watcher(client_context_t *c, int fd, int revents)
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

    ev_io *watcher = &c->io_watchers[c->nr_io_watchers];

    watcher->data = c;
    ev_io_init(watcher, on_client_io_available_cb, fd, revents);
    ev_io_start(worker_thread_get_loop(c->worker_p), watcher);

    c->nr_io_watchers++;

    return watcher;
}

/* returns 0 on success, -1 on watcher not found */
static int remove_io_watcher(client_context_t *c, int fd)
{
    size_t i = 0;
    while (i < c->nr_io_watchers && c->io_watchers[i].fd != fd)
        i++;

    if (i == c->nr_io_watchers) {
        return -1;
    }

    ev_io_stop(worker_thread_get_loop(c->worker_p), &c->io_watchers[i]);

    memmove(&c->io_watchers[i], &c->io_watchers[i + 1],
            (c->nr_io_watchers - i - 1) * sizeof(c->io_watchers[0]));

    c->nr_io_watchers--;

    return 0;
}

int client_context_create(client_context_t *cc, client_routine_t routine,
                          int sockfd, const char *descr,
                          worker_thread_t *worker)
{
    cc->worker_p = worker;
    cc->description = strdup(descr);

    if (coroutine_create(&cc->coroutine, (coroutine_func_t) routine, NULL) != OK) {
        log_error("Failed to create coroutine");
        return ERROR;
    }

    worker_thread_begin_async_modify(worker);

    cc->nr_io_watchers = 0;
    add_io_watcher(cc, sockfd, EV_READ | EV_WRITE);

    ev_async_init(&cc->drop_watcher, on_client_drop_cb);
    ev_async_start(worker_thread_get_loop(cc->worker_p), &cc->drop_watcher);

    ev_async_init(&cc->wakeup_watcher, on_client_async_wakeup_cb);
    ev_async_start(worker_thread_get_loop(cc->worker_p), &cc->wakeup_watcher);

    worker_thread_end_async_modify(worker);

    size_t worker_id = worker_thread_get_id(cc->worker_p);
    log_info("[%s, worker: %zu] New client connected", descr, worker_id);

    return OK;
}

void loop_switch_to_client(client_context_t *cc)
{
    assert(g_running_client == NULL);

    coroutine_t *client_coro = &cc->coroutine;
    coroutine_t *loop_coro = worker_thread_get_loop_coro(cc->worker_p);

    g_running_client = cc;

    size_t worker_id = worker_thread_get_id(cc->worker_p);
    log_trace("[worker: %zu] context switch: loop --> client(%s)",
              worker_id, cc->description);
    coroutine_switch(loop_coro, client_coro);
}

void async_client_wakeup(client_context_t *cc)
{
    ev_async_send(worker_thread_get_loop(cc->worker_p), &cc->wakeup_watcher);
}

static void client_switch_to_loop(client_context_t *cc)
{
    assert(g_running_client != NULL);

    coroutine_t *client_coro = &cc->coroutine;
    coroutine_t *loop_coro = worker_thread_get_loop_coro(cc->worker_p);

    g_running_client = NULL;

    size_t worker_id = worker_thread_get_id(cc->worker_p);
    log_trace("[worker: %zu] context switch: client(%s) --> loop",
              worker_id, cc->description);
    coroutine_switch(client_coro, loop_coro);
}

void client_yield(void)
{
    client_context_t *c = RUNNING_CLIENT;

    client_switch_to_loop(c);
}

void attribute_noreturn() client_drop(void)
{
    client_context_t *cc = RUNNING_CLIENT;

    /* releasing client resources happens in loop coroutine, 
     * because we can't destroy stack of currently running client coroutine
     * from itself */
    ev_async_send(worker_thread_get_loop(cc->worker_p), &cc->drop_watcher);

    /* loop in case client coroutine will wake up again, but it shouldn't */
    for (;;) {
        client_switch_to_loop(cc);
    }
}

ssize_t client_recv(fdwatcher_t w, void *buf, size_t n, int flags)
{
    ev_io *watcher = w;
    ssize_t ret;
    for (;;) {
        ret = recv(watcher->fd, buf, n, flags);
        if (ret == -1 && errno == EAGAIN)
            client_yield();
        else
            return ret;
    }
}

ssize_t client_send(fdwatcher_t w, void *buf, size_t n, int flags)
{
    ev_io *watcher = w;
    ssize_t ret;
    for (;;) {
        ret = send(watcher->fd, buf, n, flags);
        if (ret == -1 && errno == EAGAIN)
            client_yield();
        else
            return ret;
    }
}

ssize_t client_recv_buf(fdwatcher_t w, void *buf, size_t size)
{
    int old_events = client_fd_getevents(w);
    client_fd_setevents(w, EV_READ);

    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = client_recv(w, (char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        recv_size += ret;
    }

    client_fd_setevents(w, old_events);
    return size;
}

ssize_t client_send_buf(fdwatcher_t w, void *buf, size_t size)
{
    int old_events = client_fd_getevents(w);
    client_fd_setevents(w, EV_WRITE);

    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = client_send(w, (char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1 || ret == 0)
            return ret;

        sent_size += ret;
    }

    client_fd_setevents(w, old_events);
    return size;
}

ssize_t client_recv_nonblock(fdwatcher_t w, void *buf, size_t n, int flags)
{
    ev_io *watcher = w;
    ssize_t ret;

    ret = recv(watcher->fd, buf, n, flags);
    if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return NODATA;
    else
        return ret;
}

ssize_t client_send_nonblock(fdwatcher_t w, void *buf, size_t n, int flags)
{
    ev_io *watcher = w;
    ssize_t ret;

    ret = send(watcher->fd, buf, n, flags);
    if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return NODATA;
    else
        return ret;
}

fdwatcher_t client_fd_watch(int fd, int revents)
{
    client_context_t *c = RUNNING_CLIENT;

    if (set_nonblock(fd) != 0) {
        client_log_error("client_watch_fd(): Failed to set fd to nonblocking mode, %s", 
                         strerror(errno));
        client_drop();
    }

    return add_io_watcher(c, fd, revents);
}

void client_fd_unwatch(fdwatcher_t w)
{
    ev_io *watcher = w;
    client_context_t *c = RUNNING_CLIENT;

    int ret = remove_io_watcher(c, watcher->fd);
    if (ret != 0)
        client_log_warn("client_unwatch_fd(): watcher not found");
}

static void on_client_async_wakeup_cb(EV_P_ ev_async *w, int revents)
{
    client_context_t *cc = (client_context_t *) wakeup_w_2_client(w);
    loop_switch_to_client(cc);
}

static void on_client_io_available_cb(EV_P_ ev_io *w, int revents)
{
    client_context_t *cc = (client_context_t *) w->data;
    loop_switch_to_client(cc);
}

static void on_client_drop_cb(EV_P_ ev_async *w, int revents)
{
    client_context_t *cc = (client_context_t *) drop_w_2_client(w);
    log_info("[%s] Closing connection...", cc->description);

    for (size_t i = 0; i < cc->nr_io_watchers; ++i) {
        ev_io_stop(loop, &cc->io_watchers[i]);
    }

    ev_async_stop(loop, &cc->drop_watcher);

    int client_sfd = cc->io_watchers[0].fd;
    close(client_sfd);

    list_unlink(&cc->link);
    coroutine_destroy(&cc->coroutine);
    free(cc->description);
    free(cc);
}
