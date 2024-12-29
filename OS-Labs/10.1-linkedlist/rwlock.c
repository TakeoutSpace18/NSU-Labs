#include "lock.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *create_lock(void)
{
    pthread_rwlock_t *lock = malloc(sizeof(*lock));
    if (!lock) {
        perror("malloc()");
        abort();
    }
    pthread_rwlock_init(lock, NULL);

    return lock;
}

void read_lock(void *lock)
{
    pthread_rwlock_rdlock(lock);
}

void write_lock(void *lock)
{
    pthread_rwlock_wrlock(lock);
}

void unlock(void *lock)
{
    pthread_rwlock_unlock(lock);
}
