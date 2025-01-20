#include <signal.h>
#define _GNU_SOURCE
#include "client_context.h"

#include <ev.h>

#include "c.h"
#include "dns.h"
#include "log.h"
#include "list.h"
#include "utils.h"
#include "socket.h"
#include "dynarray.h"
#include "coroutine.h"
#include "worker_thread.h"

static void on_client_async_wakeup_cb(EV_P_ struct ev_async *w, int revents);
static void on_client_io_available_cb(EV_P_ struct ev_io *w, int revents);
static void on_client_drop_cb(EV_P_ struct ev_async *w, int revents);

static fdwatcher_t *fdwatcher_create_internal(client_context_t *cc, int fd, int events)
{
    fdwatcher_t *fdw = dynarray_emplace_back(&cc->fdwatchers);

    fdw->buffer = malloc(FDWATCHER_DEFAULT_BUFFER_SZ);
    if (!fdw->buffer) {
        coroutine_log_fatal("out of memory");
        abort();
    }
    fdw->buffer_size = FDWATCHER_DEFAULT_BUFFER_SZ;
    fdw->nr_buffered = 0;

    ev_io_init(&fdw->io, on_client_io_available_cb, fd, events);
    ev_io_start(worker_thread_get_loop(cc->worker_p), &fdw->io);
    fdw->io.data = cc;

    return fdw;
}

static void fdwatcher_destroy(client_context_t *cc, fdwatcher_t *fdw)
{
    ev_io_stop(worker_thread_get_loop(cc->worker_p), &fdw->io);
    free(fdw->buffer);
}

static void remove_fdwatcher(client_context_t *cc, fdwatcher_t *fdw)
{
    size_t i;
    for (i = 0; i < dynarray_size(&cc->fdwatchers); i++) {
        if (dynarray_at(&cc->fdwatchers, i) == fdw) {
            break;
        }
    }

    if (i == dynarray_size(&cc->fdwatchers)) {
        coroutine_log_fatal("remove_fdwatcher(): watcher not found");
        abort();
    }

    fdwatcher_destroy(cc, fdw);
    dynarray_remove(&cc->fdwatchers, i);
}

int client_context_create(client_context_t *cc, client_routine_t routine,
                          void *routine_arg,
                          int sockfd, const char *descr,
                          worker_thread_t *worker)
{
    cc->worker_p = worker;

    if (coroutine_create(&cc->coroutine, (coroutine_func_t) routine, routine_arg) != OK) {
        log_error("Failed to create coroutine");
        return ERROR;
    }

    /* Make client_context struct be accessible from coroutine context.
     * This is used by RUNNING_CLIENT macro */
    cc->coroutine.data = cc;

    char name[256];
    snprintf(name, 256, "worker %zu :: %s", worker_thread_get_id(worker), descr);
    coroutine_set_name(&cc->coroutine, name);

    worker_thread_begin_async_modify(worker);

    dynarray_create(&cc->fdwatchers, sizeof(fdwatcher_t));
    if (dynarray_reserve(&cc->fdwatchers, 5) != OK) {
        log_error("out of memory");
        abort();
    }

    fdwatcher_create_internal(cc, sockfd, 0);

    ev_async_init(&cc->drop_watcher, on_client_drop_cb);
    ev_async_start(worker_thread_get_loop(cc->worker_p), &cc->drop_watcher);

    ev_async_init(&cc->wakeup_watcher, on_client_async_wakeup_cb);
    ev_async_start(worker_thread_get_loop(cc->worker_p), &cc->wakeup_watcher);

    worker_thread_end_async_modify(worker);

    size_t worker_id = worker_thread_get_id(cc->worker_p);
    log_info("[worker %zu :: %s] New client connected", worker_id, descr);

    return OK;
}

void loop_switch_to_client(client_context_t *cc)
{
    coroutine_t *client_coro = &cc->coroutine;
    coroutine_t *loop_coro = worker_thread_get_loop_coro(cc->worker_p);

    coroutine_switch(loop_coro, client_coro);
}

void client_switch_to_loop(client_context_t *cc)
{
    coroutine_t *client_coro = &cc->coroutine;
    coroutine_t *loop_coro = worker_thread_get_loop_coro(cc->worker_p);

    coroutine_switch(client_coro, loop_coro);
}

void async_client_wakeup(client_context_t *cc)
{
    ev_async_send(worker_thread_get_loop(cc->worker_p), &cc->wakeup_watcher);
}

void client_yield(void)
{
    client_context_t *c = RUNNING_CLIENT;

    client_switch_to_loop(c);
}

void attribute_noreturn() client_drop(void)
{
    client_context_t *cc = RUNNING_CLIENT;

    coroutine_log_info("Closing connection...");

    /* releasing client resources happens in loop coroutine, 
     * because we can't destroy stack of currently running client coroutine
     * from itself */
    ev_async_send(worker_thread_get_loop(cc->worker_p), &cc->drop_watcher);

    /* loop in case client coroutine will wake up again, but it shouldn't */
    for (;;) {
        client_switch_to_loop(cc);
    }
}

ssize_t client_recv(fdwatcher_t *fdw, void *buf, size_t size, int flags)
{
    int old_events = client_fd_getevents(fdw);
    client_fd_setevents(fdw, EV_READ);

    ssize_t ret;
    for (;;) {
        ret = client_recv_nonblock(fdw, buf, size, flags);
        if (ret == -EAGAIN) {
            client_yield();
        }
        else {
            client_fd_setevents(fdw, old_events);
            return ret;
        }
    }

}

ssize_t client_send(fdwatcher_t *fdw, const void *buf, size_t size, int flags)
{
    int old_events = client_fd_getevents(fdw);
    client_fd_setevents(fdw, EV_WRITE);

    ssize_t ret;
    for (;;) {
        ret = client_send_nonblock(fdw, buf, size, flags);
        if (ret == -EAGAIN) {
            client_yield();
        }
        else {
            client_fd_setevents(fdw, old_events);
            return ret;
        }
    }
}

ssize_t client_recv_all(fdwatcher_t *fdw, void *buf, size_t size)
{
    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = client_recv(fdw, (char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1 || ret == 0) {
            return ret;
        }

        recv_size += ret;
    }

    return size;
}

ssize_t client_send_all(fdwatcher_t *fdw, const void *buf, size_t size)
{
    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = client_send(fdw, (char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1 || ret == 0) {
            return ret;
        }

        sent_size += ret;
    }
    return size;
}

ssize_t client_recv_nonblock(fdwatcher_t *fdw, void *buf, size_t size, int flags)
{
    /* if buffered data is enough to fulfill the request */
    if (fdw->nr_buffered >= size) {
        memcpy(buf, fdw->buffer, size);
        fdw->nr_buffered -= size;
        memmove(fdw->buffer, fdw->buffer + size, fdw->nr_buffered);

        return size;
    }

    /* firstly give away the buffered data */
    if (fdw->nr_buffered > 0) {
        memcpy(buf, fdw->buffer, fdw->nr_buffered);
        size -= fdw->nr_buffered;
        buf = (char *) buf + fdw->nr_buffered;
        fdw->nr_buffered = 0;
    }

    ssize_t ret;
    for (;;) {
        ret = recv(fdw->io.fd, buf, size, flags);
        if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            return -EAGAIN;
        else
            return ret;
    }
}

ssize_t client_send_nonblock(fdwatcher_t *fdw, const void *buf, size_t size, int flags)
{
    ssize_t ret;

    ret = send(fdw->io.fd, buf, size, flags);
    if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        return -EAGAIN;
    else
        return ret;
}

static void *realloc_check_err(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        coroutine_log_fatal("out of memory");
        exit(EXIT_FAILURE);
    }

    return new_ptr;
}

static ssize_t recv_blocking(int sockfd, void *buf, size_t len, int flags)
{
    ssize_t ret;

    for (;;) {
        ret = recv(sockfd, buf, len, flags);
        if (ret == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            client_yield();
        }
        else {
            break;
        }
    }

    return ret;
}

ssize_t client_recv_until(fdwatcher_t *fdw, void *buf, size_t size,
                          const void *delim, size_t delim_size)
{
    if (size > fdw->buffer_size) {
        fdw->buffer = realloc_check_err(fdw->buffer, size);
        fdw->buffer_size = size;
    }

    void *delim_location = NULL;
    while ((delim_location = memmem(fdw->buffer, fdw->nr_buffered, delim, delim_size)) == NULL) {
        if (fdw->nr_buffered >= size) {
            return NODATA;
        }
        
        ssize_t rcvd = recv_blocking(fdw->io.fd, fdw->buffer + fdw->nr_buffered,
                                     size - fdw->nr_buffered, 0);

        if (rcvd == -1 || rcvd == 0) {
            return rcvd;
        }

        fdw->nr_buffered += rcvd;
    }

    size_t ret_size = (char *) delim_location + delim_size - fdw->buffer;
    memcpy(buf, fdw->buffer, ret_size);
    fdw->nr_buffered -= ret_size;
    memmove(fdw->buffer, fdw->buffer + ret_size, fdw->nr_buffered);

    return ret_size;
}

fdwatcher_t *client_fdwatcher_create(int fd, int events)
{
    client_context_t *c = RUNNING_CLIENT;

    if (set_nonblock(fd) != 0) {
        coroutine_log_error("client_fdwatcher_create(): "
                            "Failed to set fd to nonblocking mode, %s", 
                             strerror(errno));
        return NULL;
    }

    return fdwatcher_create_internal(c, fd, events);
}

void client_fdwatcher_destroy(fdwatcher_t *fdw)
{
    client_context_t *c = RUNNING_CLIENT;
    remove_fdwatcher(c, fdw);
}

bool fdwatcher_read_pending(fdwatcher_t *fdw)
{
    size_t bytes_pending;
	int ret = ioctl(fdw->io.fd, FIONREAD, &bytes_pending);
	if (ret == -1) {
		return -1;
	}

	return bytes_pending > 0;
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

    fdwatcher_t *client_fdw = dynarray_at(&cc->fdwatchers, 0);
    proper_socket_close(client_fdw->io.fd);

    fdwatcher_t *fdw;
    dynarray_foreach(fdw, &cc->fdwatchers) {
        fdwatcher_destroy(cc, fdw);
    }

    ev_async_stop(loop, &cc->drop_watcher);
    ev_async_stop(loop, &cc->wakeup_watcher);

    list_unlink(&cc->link);
    coroutine_destroy(&cc->coroutine);
    dynarray_destroy(&cc->fdwatchers);
    free(cc);
}
