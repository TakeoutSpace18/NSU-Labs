#ifndef CLIENT_CONTEXT_H
#define CLIENT_CONTEXT_H

#include <ev.h>
#include <threads.h>

#include "c.h"
#include "dns.h"
#include "list.h"
#include "coroutine.h"
#include "worker_thread.h"

#define MAX_CLIENT_FDWATCHERS 5
#define FDWATCHER_DEFAULT_BUFFER_SZ 512

typedef void (*client_routine_t)(void *);

typedef struct client_fdwatcher {
    struct ev_io io;

    /* buffer is used for recv_until request for saving
     * received data after the demimeter string */

    /* TODO: replace with buffer from buffer.h */
    char *buffer;
    size_t buffer_size;
    size_t nr_buffered;
} fdwatcher_t;

typedef struct client_context {
    /* fds, that are watched by libev 
     * fdwatchers[0] is always the accepted client socket */
    size_t nr_fdwatchers;
    fdwatcher_t fdwatchers[MAX_CLIENT_FDWATCHERS];

    ev_async drop_watcher;
    ev_async wakeup_watcher;

    coroutine_t coroutine;

    list_t link;

    worker_thread_t *worker_p;
} client_context_t;

#define RUNNING_CLIENT (client_context_t *) coroutine_get_current()->data
#define IN_CLIENT_CONTEXT (coroutine_get_current()->data != NULL)

/* access holder structure from watcher pointer */
#define io_w_2_client(p) container_of(p, client_context_t, io_watcher)
#define drop_w_2_client(p) container_of(p, client_context_t, drop_watcher)
#define wakeup_w_2_client(p) container_of(p, client_context_t, wakeup_watcher)

int client_context_create(client_context_t *cc, client_routine_t routine,
                          void *routine_arg,
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

/* Make file descriptor to be watched by libev backend.
 * Events are passed to libev, can be either EV_READ, EV_WRITE or both.
 * Client coroutine will be woken up when any of specified events
 * happen on fd */
fdwatcher_t *client_fdwatcher_create(int fd, int events);
void client_fdwatcher_destroy(fdwatcher_t *fdw);

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
ssize_t client_recv(fdwatcher_t *fdw, void *buf, size_t size, int flags);
ssize_t client_send(fdwatcher_t *fdw, const void *buf, size_t size, int flags);

/* Receive or send buffer of given size.
 * Client blocks and waits until whole buffer is tramsmitted.
 * Should be called after fd has been registered with
 * client_watch_fd() and before client_unwatch_fd().
 * Watched events are set internally and restored afterwards.
 * RETURNS:
 *      size, on success 
 *      0, on closed socket,
 *      -1, on other error (sets errno) */
ssize_t client_recv_all(fdwatcher_t *fdw, void *buf, size_t size);
ssize_t client_send_all(fdwatcher_t *fdw, const void *buf, size_t size);

/* Receive or send data, returns NODATA if it would block. 
 * Clients can use client_yield() to implement custom logic
 * of returning control to server.
 * Clients should set watcher events either to EV_READ or EV_WRITE
 * before calling this routines.
 * RETURNS:
 *      num of bytes read or sent, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno)
 *      -EAGAIN, if the call would block */
ssize_t client_recv_nonblock(fdwatcher_t *fdw, void *buf, size_t size, int flags);
ssize_t client_send_nonblock(fdwatcher_t *fdw, const void *buf, size_t size, int flags);

#define NODATA -2

/* Receive data until delimeter is found
 * RETURNS:
 *      num of bytes read, on success
 *      0, on closed socket,
 *      -1, on other error (sets errno)
 *      NODATA, if delimeter does not fit in supplied buffer */
ssize_t client_recv_until(fdwatcher_t *fdw, void *buf, size_t size,
                          const void *delim, size_t delim_size);

/* set events on which client coroutine will be woken up */
static inline void client_fd_setevents(fdwatcher_t *fdw, int events)
{
    client_context_t *c = fdw->io.data;
    struct ev_loop *loop = worker_thread_get_loop(c->worker_p);

    ev_io_stop(loop, &fdw->io);
    ev_io_set(&fdw->io, fdw->io.fd, events);
    ev_io_start(loop, &fdw->io);
}

bool fdwatcher_read_pending(fdwatcher_t *fdw);

static inline int client_fd_getevents(fdwatcher_t *fdw)
{
    return fdw->io.events;
}

static inline fdwatcher_t *client_fdwatcher(void)
{
    client_context_t *c = RUNNING_CLIENT;
    return &c->fdwatchers[0];
}

#endif /* CLIENT_CONTEXT_H */
