#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <execinfo.h>
#include <stdbool.h>

#include "libmythread.h"

bool all_created = false;

static void *
thread_main(void *arg)
{
    while (!all_created);
        
    uint64_t num = (uint64_t) arg;
    printf("%lu ", num);
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

    all_created = true;

    printf("[main] joining threads...\n");
    for (uint64_t i = 0; i < NUM_THREADS; ++i) {
        if (MyThreadJoin(&threads[i], NULL) != OK) {
            fprintf(stderr, "%s\n", MyThreadErrorDescr());
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
