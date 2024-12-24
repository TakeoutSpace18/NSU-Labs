#ifndef LOG_SETUP_H
#define LOG_SETUP_H

#include <log.h>
#include <pthread.h>

static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

static void __logger_lock_cb(bool lock, void *udata)
{
    pthread_mutex_t *mtx = (pthread_mutex_t *) udata;

    if (lock) {
        pthread_mutex_lock(mtx);
    }
    else {
        pthread_mutex_unlock(mtx);
    }
}

static inline void logger_setup(void)
{
    log_set_lock(__logger_lock_cb, &log_lock);
}

#endif /* !LOG_SETUP_H */

