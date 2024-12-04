#ifndef SERVER_H
#define SERVER_H

#include <ev.h>

#include "c.h"
#include "dns.h"
#include "coro.h"
#include "list.h"
#include "log.h"

#define MAX_CLIENT_IO_WATCHERS 5

typedef struct server server_t;
typedef struct client client_t;
typedef struct client_io_watcher client_io_watcher_t;
typedef void (*client_routine_t)(void);

extern client_t *g_running_client;

struct server {
    ev_io accept_watcher;

    /* coroutine that accepts client connections */
    coro_context context;
    struct ev_loop *loop;

    list_t clients;
    client_routine_t client_routine;

    dns_resolver_t dns;
};

#define accept_w_2_server(p) container_of(p, server_t, accept_watcher)

struct client {
    /* fds, that are watched by libev 
     * io_watchers[0] is always accepted client socket */
    size_t nr_io_watchers;
    ev_io io_watchers[MAX_CLIENT_IO_WATCHERS];

    ev_async drop_watcher;

    coro_context context;
    struct coro_stack stack;

    list_t link;

    char *description;

    server_t *server_p;
};

/* access holder structure from watcher pointer */
#define io_w_2_client(p) container_of(p, client_t, io_watcher)
#define drop_w_2_client(p) container_of(p, client_t, drop_watcher)

int server_init(server_t *s, uint16_t port, client_routine_t routine);
void server_run(server_t *s);
void server_finalize(server_t *s);
void server_switch_to_client(client_t *client);

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
    client_t *client = watcher->data;

    ev_io_stop(client->server_p->loop, watcher);
    ev_io_set(watcher, watcher->fd, events);
    ev_io_start(client->server_p->loop, watcher);
}

static inline int client_fd_getevents(fdwatcher_t w)
{
    ev_io *watcher = w;
    return watcher->events;
}

/* client socket fd */
static inline int client_sfd(void)
{
    client_t *c = g_running_client;
    return c->io_watchers[0].fd;
}

static inline fdwatcher_t client_fdwatcher(void)
{
    client_t *c = g_running_client;
    return &c->io_watchers[0];
}

__attribute__ ((format (printf, 4, 5)))
static inline void
client_log(int level, const char *file, int line, const char *fmt, ...)
{
    client_t *c = g_running_client;

    static char extended_fmt[512] = { 0 };
    snprintf(extended_fmt, 512, "[%s] %s", c->description, fmt);

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

#endif /* SERVER_H */
