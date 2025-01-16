#include "client_cond.h"

#include <pthread.h>

#include "client_context.h"
#include "coroutine.h"

void client_cond_create(client_cond_t *cond)
{
    dynarray_create(&cond->waiters, sizeof(client_context_t *));
    int err = dynarray_reserve(&cond->waiters, 32);
    if (err == ENOMEM) {
        coroutine_log_fatal("out of memory");
        abort();
    }

    pthread_mutex_init(&cond->lock, NULL);
}

void client_cond_destroy(client_cond_t *cond)
{
    dynarray_destroy(&cond->waiters);
    pthread_mutex_destroy(&cond->lock);
}

void client_cond_wait(client_cond_t *cond)
{
    client_context_t *cc = RUNNING_CLIENT;

    pthread_mutex_lock(&cond->lock);

    int err = dynarray_push_back(&cond->waiters, &cc);
    if (err == ENOMEM) {
        coroutine_log_fatal("out of memory");
        abort();
    }

    pthread_mutex_unlock(&cond->lock);

    client_yield();
}

void client_cond_signal(client_cond_t *cond)
{
    pthread_mutex_lock(&cond->lock);

    client_context_t **waiter;
    dynarray_foreach(waiter, &cond->waiters) {
        async_client_wakeup(*waiter);
    }

    dynarray_reset(&cond->waiters);
    pthread_mutex_unlock(&cond->lock);
}
