#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <string.h>

#include "myspinlock.h"
#include "mymutex.h"

#define NUM_THREADS 4
#define TARGET_SUM 10000000

uint64_t g_sum = 0;
mymutex_t mutex;
myspinlock_t spinlock;

static void *adder_no_locking(void *arg)
{
    uint64_t count = (uint64_t) arg;

    for (uint64_t i = 0; i < count; ++i) {
        g_sum += 1;
    }

    return EXIT_SUCCESS;
}

static void *adder_mymutex(void *arg)
{
    uint64_t count = (uint64_t) arg;

    for (uint64_t i = 0; i < count; ++i) {
        mymutex_lock(&mutex);
        g_sum += 1;
        mymutex_unlock(&mutex);
    }

    return EXIT_SUCCESS;
}

static void *adder_myspinlock(void *arg)
{
    uint64_t count = (uint64_t) arg;

    for (uint64_t i = 0; i < count; ++i) {
        myspinlock_lock(&spinlock);
        g_sum += 1;
        myspinlock_unlock(&spinlock);
    }

    return EXIT_SUCCESS;
}

static inline void create_thread(pthread_t *thread, void *(*routine)(void*), void *arg)
{
    int err = pthread_create(thread, NULL, routine, arg);
    if (err != 0) {
        fprintf(stderr, "pthread_create(): %s\n", strerror(err));
        abort();
    }
}

static inline void join_thread(pthread_t thread)
{
    int err = pthread_join(thread, NULL);
    if (err != 0) {
        fprintf(stderr, "pthread_join(): %s\n", strerror(err));
        abort();
    }
}

static void run_adders(const char *caption, void *(*routine)(void *))
{
    pthread_t threads[NUM_THREADS];
    uint64_t count = TARGET_SUM / NUM_THREADS;

    printf("--- %s ---\n", caption);
    g_sum = 0;
    for (int i = 0; i < NUM_THREADS; ++i) {
        create_thread(&threads[i], routine, (void *) count);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        join_thread(threads[i]);
    }
    printf("sum = %lu\n\n", g_sum);

}

int main(void) 
{
    mymutex_init(&mutex);
    myspinlock_init(&spinlock);

    run_adders("NO LOCKING", adder_no_locking);
    run_adders("MY MUTEX", adder_mymutex);
    run_adders("MY SPINLOCK", adder_myspinlock);

    return EXIT_SUCCESS;
}
