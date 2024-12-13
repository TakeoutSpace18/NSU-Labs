#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <ev.h>
#include <threads.h>

#include "c.h"
#include "dns.h"
#include "list.h"
#include "log.h"
#include "coroutine.h"
#include "worker_thread.h"

#define MAX_CLIENT_IO_WATCHERS 5

typedef struct client_io_watcher client_io_watcher_t;
typedef void (*client_routine_t)(void);

typedef struct client_context {
    /* fds, that are watched by libev 
     * io_watchers[0] is always accepted client socket */
    size_t nr_io_watchers;
    ev_io io_watchers[MAX_CLIENT_IO_WATCHERS];

    ev_async drop_watcher;
    ev_async wakeup_watcher;

    coroutine_t coroutine;

    list_t link;

    char *description;

    worker_thread_t *worker_p;
} client_context_t;

extern thread_local client_context_t *g_running_client;

#define RUNNING_CLIENT g_running_client
#define IN_CLIENT_CONTEXT (g_running_client != NULL)

/* access holder structure from watcher pointer */
#define io_w_2_client(p) container_of(p, client_context_t, io_watcher)
#define drop_w_2_client(p) container_of(p, client_context_t, drop_watcher)
#define wakeup_w_2_client(p) container_of(p, client_context_t, wakeup_watcher)

int client_context_create(client_context_t *cc, client_routine_t routine,
                          int sockfd, const char *descr,
                          worker_thread_t *worker);

void loop_switch_to_client(client_context_t *cc);
void client_switch_to_loop(client_context_t *cc);

/* used to give control to client from another thread */
void async_client_wakeup(client_context_t *cc);

/* functions below should be called from the context of client coroutine */

/* close connection with client */
void client_drop(void);

/* Return control to the server coroutine */
void client_yield(void);

typedef struct ev_io * fdwatcher_t;

/* Make file descriptor to be watched by libev backend.
 * Events are passed to libev, can be either EV_READ or EV_WRITE.
 * Client coroutine will be woken up when any of specified events
 * happen on fd */
fdwatcher_t client_fd_watch(int fd, int revents);
void client_fd_unwatch(fdwatcher_t w);

/* Receive or send data, but internally switch
 * to other coroutines when no data is available. 
 * Should be called after fd has been registered with
 * client_watch_fd() and before client_unwatch_fd().
 * Clients should set watcher events either to EV_READ or EV_WRITE
 * before calling this routines.
 * RETURNS:
 *      num of bytes read or sent, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno) */
ssize_t client_recv(fdwatcher_t w, void *buf, size_t n, int flags);
ssize_t client_send(fdwatcher_t w, void *buf, size_t n, int flags);

/* Receive or send buffer of given size.
 * Client blocks and waits until whole buffer is tramsmitted.
 * Should be called after fd has been registered with
 * client_watch_fd() and before client_unwatch_fd().
 * Watched events are set internally and restored afterwards.
 * RETURNS:
 *      size, on success 
 *      0, on closed socket,
 *      -1, on other error (sets errno) */
ssize_t client_recv_buf(fdwatcher_t w, void *buf, size_t size);
ssize_t client_send_buf(fdwatcher_t w, void *buf, size_t size);

#define NODATA (-2)

/* Receive or send data, returns NODATA if it would block. 
 * Clients can use client_yield() to implement custom logic
 * of returning control to server.
 * Clients should set watcher events either to EV_READ or EV_WRITE
 * before calling this routines.
 * RETURNS:
 *      num of bytes read or sent, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno)
 *      NODATA, if the call would block */
ssize_t client_recv_nonblock(fdwatcher_t w, void *buf, size_t n, int flags);
ssize_t client_send_nonblock(fdwatcher_t w, void *buf, size_t n, int flags);

/* set events on which client coroutine will be woken up */
static inline void client_fd_setevents(fdwatcher_t w, int events)
{
    ev_io *watcher = w;
    client_context_t *c = watcher->data;

    ev_io_stop(worker_thread_get_loop(c->worker_p), watcher);
    ev_io_set(watcher, watcher->fd, events);
    ev_io_start(worker_thread_get_loop(c->worker_p), watcher);
}

static inline int client_fd_getevents(fdwatcher_t w)
{
    ev_io *watcher = w;
    return watcher->events;
}

/* client socket fd */
static inline int client_sfd(void)
{
    client_context_t *c = RUNNING_CLIENT;
    return c->io_watchers[0].fd;
}

static inline fdwatcher_t client_fdwatcher(void)
{
    client_context_t *c = RUNNING_CLIENT;
    return &c->io_watchers[0];
}

__attribute__ ((format (printf, 4, 5)))
static inline void
client_log(int level, const char *file, int line, const char *fmt, ...)
{
    client_context_t *cc = RUNNING_CLIENT;

    static char extended_fmt[512] = { 0 };
    snprintf(extended_fmt, 512, "[%s, worker: %zu] %s",
             cc->description, worker_thread_get_id(cc->worker_p), fmt);

    va_list ap;
    va_start(ap, fmt);
    vlog_log(level, file, line, extended_fmt, ap);
    va_end(ap);
}

#define client_log_trace(...) client_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define client_log_debug(...) client_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define client_log_info(...)  client_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define client_log_warn(...)  client_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define client_log_error(...) client_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define client_log_fatal(...) client_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif /* CLIENT_CONTEXT_H */
