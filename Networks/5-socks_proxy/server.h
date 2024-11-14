#ifndef SERVER_H
#define SERVER_H

#include <ev.h>

#include "coro.h"
#include "list.h"
#include "c.h"
#include "log.h"
#include <stdio.h>

typedef struct server server_t;
typedef struct client client_t;
typedef void (*client_routine_t)(void);

extern client_t *g_running_client;

struct server {
    ev_io accept_watcher; /* mandatory to be the first field */

    /* coroutine that accepts client connections */
    coro_context context;
    struct ev_loop *loop;

    list_t clients;
    client_routine_t client_routine;
};

struct client {
    ev_io    io_watcher; 
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


int server_create(server_t *s, uint16_t port, client_routine_t routine);
void server_run(server_t *s);

/* functions below should be called from the context of client coroutine */
void client_drop(void);
void client_yield(void);

/* receive or send data with respect to other client coroutines,
 * returns -1 in case of error */
ssize_t client_recv(void *buf, size_t n, int flags);
ssize_t client_send(void *buf, size_t n, int flags);

/* receive or send whole buffer of given size or return -1 */
ssize_t client_recv_buf(void *buf, size_t size);
ssize_t client_send_buf(void *buf, size_t size);

static inline int client_sockfd(void)
{
    client_t *c = g_running_client;
    return c->io_watcher.fd;
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
