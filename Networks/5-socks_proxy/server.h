#ifndef SERVER_H
#define SERVER_H

#include <ev.h>

#include "dns.h"
#include "coro.h"
#include "list.h"
#include "c.h"
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

struct client_io_watcher {
    ev_io w;
    client_t *client;
};

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

/* Receive or send data, but internally switch
 * to other coroutines when no data is available. 
 * Should be called after fd has been registered with
 * client_watch_fd() and before client_unwatch_fd() 
 * RETURNS:
 *      num of bytes read or sent, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno) */
ssize_t client_recv(int fd, void *buf, size_t n, int flags);
ssize_t client_send(int fd, void *buf, size_t n, int flags);

/* Receive or send buffer of given size.
 * Client blocks and waits until whole buffer is tramsmitted.
 * Should be called after fd has been registered with
 * client_watch_fd() and before client_unwatch_fd() 
 * RETURNS:
 *      size, on success 
 *      0, on closed socket,
 *      -1, on other error (sets errno) */
ssize_t client_recv_buf(int fd, void *buf, size_t size);
ssize_t client_send_buf(int fd, void *buf, size_t size);

#define NODATA (-2)

/* Receive or send data, returns NODATA if it would block. 
 * Clients can use client_yield() to implement custom logic
 * of returning control to server.
 * RETURNS:
 *      num of bytes read or sent, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno)
 *      NODATA, if the call would block */
ssize_t client_recv_nonblock(int fd, void *buf, size_t n, int flags);
ssize_t client_send_nonblock(int fd, void *buf, size_t n, int flags);

/* Make file descriptor to be watched by libev backend.
 * Client coroutine will be woken up when data on socket is available. */
void client_watch_fd(int fd);
void client_unwatch_fd(int fd);

static inline int client_sfd(void)
{
    client_t *c = g_running_client;
    return c->io_watchers[0].fd;
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
