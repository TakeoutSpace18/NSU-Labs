#define _GNU_SOURCE
#include "worker_thread.h"

#include <pthread.h>
#include <ev.h>

#include "c.h"
#include "client_context.h"
#include "coroutine.h"
#include "list.h"
#include "log.h"
#include "dns.h"

static void *worker_thread_main(void *arg);

static void loop_acquire_cb(EV_P)
{
    worker_thread_t *wt = ev_userdata(EV_A);
    pthread_mutex_lock(&wt->loop_lock);
}

static void loop_release_cb(EV_P)
{
    worker_thread_t *wt = ev_userdata(EV_A);
    pthread_mutex_unlock(&wt->loop_lock);
}

static void loop_wakeup_cb(EV_P_ ev_async *w, int revents)
{
    /* just used for the side effects */
}

int worker_thread_create(worker_thread_t *wt, size_t id)
{
    wt->id = id;

    pthread_mutex_init(&wt->loop_lock, NULL);
    wt->loop = ev_loop_new(0);
    ev_set_userdata(wt->loop, wt);
    ev_set_loop_release_cb(wt->loop, loop_release_cb, loop_acquire_cb);

    ev_async_init(&wt->loop_wakeup, loop_wakeup_cb);
    ev_async_start(wt->loop, &wt->loop_wakeup);

    dns_resolver_create(&wt->dns, wt->loop);

    list_init(&wt->clients);
    wt->nr_clients = 0;
    pthread_mutex_init(&wt->clients_lock, NULL);

    int err = pthread_create(&wt->thread, NULL, worker_thread_main, wt);
    if (err != 0) {
        log_error("pthread_create() failed: %s", strerror(err));
        return ERROR;
    }

    return OK;
}

void worker_thread_stop(worker_thread_t *wt)
{
    pthread_cancel(wt->thread);
}

void worker_thread_destroy(worker_thread_t *wt)
{
    int err = pthread_join(wt->thread, NULL);
    if (err != 0) {
        log_error("pthread_join(): %s", strerror(err));
    }

    ev_loop_destroy(wt->loop);
    dns_resolver_destroy(&wt->dns);
}

void worker_thread_add_client(worker_thread_t *wt, client_context_t *cc)
{
    pthread_mutex_lock(&wt->clients_lock);
    list_push_front(&wt->clients, &cc->link);
    wt->nr_clients++;
    pthread_mutex_unlock(&wt->clients_lock);
}

void worker_thread_remove_client(worker_thread_t *wt, client_context_t *cc)
{
    pthread_mutex_lock(&wt->clients_lock);
    list_unlink(&cc->link);
    wt->nr_clients--;
    pthread_mutex_unlock(&wt->clients_lock);
}

static void *worker_thread_main(void *arg)
{
    worker_thread_t *wt = (worker_thread_t *) arg;

    coroutine_create_initial(&wt->loop_coro);

    char name[64];
    snprintf(name, 64, "worker %zu :: loop", worker_thread_get_id(wt));
    coroutine_set_name(&wt->loop_coro, name);

    log_info("[worker %zu] Started, TID: %i", worker_thread_get_id(wt), gettid());

    loop_acquire_cb(wt->loop);
    ev_run(wt->loop, 0);
    loop_release_cb(wt->loop);

    return NULL;
}
