#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <execinfo.h>

#include "libmythread.h"

static void *
thread_main(void *arg)
{
    uint64_t num = (uint64_t) arg;
    printf("%lu\n", num);
    return EXIT_SUCCESS;
}

#define NUM_THREADS 100

int main(void)
{
    MyThread_t threads[NUM_THREADS];

    printf("[main] creating threads...\n");
    for (uint64_t i = 0; i < NUM_THREADS; ++i) {
        if (MyThreadCreate(&threads[i], thread_main, (void *) i) != OK) {
            fprintf(stderr, "%s\n", MyThreadErrorDescr());
            return EXIT_FAILURE;
        }
    }

    printf("[main] joining threads...\n");
    for (uint64_t i = 0; i < NUM_THREADS; ++i) {
        if (MyThreadJoin(&threads[i], NULL) != OK) {
            fprintf(stderr, "%s\n", MyThreadErrorDescr());
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
