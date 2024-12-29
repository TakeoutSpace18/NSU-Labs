#include "lock.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *create_lock(void)
{
    pthread_mutex_t *lock = malloc(sizeof(*lock));
    if (!lock) {
        perror("malloc()");
        abort();
    }
    pthread_mutex_init(lock, NULL);

    return lock;
}

void read_lock(void *lock)
{
    pthread_mutex_lock(lock);
}

void write_lock(void *lock)
{
    pthread_mutex_lock(lock);
}

void unlock(void *lock)
{
    pthread_mutex_unlock(lock);
}
