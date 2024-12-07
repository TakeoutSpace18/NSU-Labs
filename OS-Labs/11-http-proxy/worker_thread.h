#ifndef WORKER_THREAD_H
#define WORKER_THREAD_H

#include "c.h"
#include "dns.h"
#include "list.h"
#include "coroutine.h"
#include <ev.h>
#include <pthread.h>

typedef struct worker_thread {
    pthread_t thread;

    struct ev_loop *loop;
    pthread_mutex_t loop_lock;
    coroutine_t loop_coro;
    ev_async loop_wakeup;

    dns_resolver_t dns;

    size_t id;

} worker_thread_t;

int worker_thread_create(worker_thread_t *wt, size_t id);

void worker_thread_stop(worker_thread_t *wt);
void worker_thread_destroy(worker_thread_t *wt);

/* all ev_loop modifications from another thread context 
 * should happen between these 2 calls:*/
static inline void
worker_thread_begin_async_modify(worker_thread_t *wt)
{
    pthread_mutex_lock(&wt->loop_lock);
}

static inline void
worker_thread_end_async_modify(worker_thread_t *wt)
{
    ev_async_send(wt->loop, &wt->loop_wakeup);
    pthread_mutex_unlock(&wt->loop_lock);
}

static inline struct ev_loop *
worker_thread_get_loop(worker_thread_t *wt)
{
    return wt->loop;
}

static inline coroutine_t *
worker_thread_get_loop_coro(worker_thread_t *wt)
{
    return &wt->loop_coro;
}

static inline dns_resolver_t *
worker_thread_get_dns_resolver(worker_thread_t *wt)
{
    return &wt->dns;
}

static inline size_t
worker_thread_get_id(const worker_thread_t *wt)
{
    return wt->id;
}

#endif /* !WORKER_THREAD_H */
