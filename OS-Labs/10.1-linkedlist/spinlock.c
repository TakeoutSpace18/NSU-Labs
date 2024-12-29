#include "lock.h"

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *create_lock(void)
{
    pthread_spinlock_t *lock = malloc(sizeof(*lock));
    if (!lock) {
        perror("malloc()");
        abort();
    }
    pthread_spin_init(lock, NULL);

    return lock;
}

void read_lock(void *lock)
{
    pthread_spin_lock(lock);
}

void write_lock(void *lock)
{
    pthread_spin_lock(lock);
}

void unlock(void *lock)
{
    pthread_spin_unlock(lock);
}
