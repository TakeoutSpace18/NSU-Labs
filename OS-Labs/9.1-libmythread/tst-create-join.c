#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <execinfo.h>

#include "libmythread.h"

/* this thread does work for some time */
static void *
thread_one(void *arg)
{
    printf("[thread_one] started\n");

    for (int i = 0; i < 5; ++i) {
        printf("[thread_one] running thread loop... [%i]\n", i);
        usleep(500000);
    }

    printf("[thread_one] finished\n");

    return EXIT_SUCCESS;
}

/* this thread immediately returns arg value */
static void *
thread_two(void *arg)
{
    return arg;
    printf("[thread_two] returning...\n");
}

int main(void)
{
    MyThread_t threads[2];

    printf("[main] creating thread_one...\n");
    if (MyThreadCreate(&threads[0], thread_one, NULL) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    printf("[main] creating thread_two...\n");
    if (MyThreadCreate(&threads[1], thread_two, (void *) 42) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    printf("[main] joining thread_one...\n");
    if (MyThreadJoin(&threads[0], NULL) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    void *ret;
    printf("[main] joining thread_two...\n");
    if (MyThreadJoin(&threads[1], &ret) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }
    printf("[main] thread_two returned %lu\n", (uint64_t) ret);

    return EXIT_SUCCESS;
}
