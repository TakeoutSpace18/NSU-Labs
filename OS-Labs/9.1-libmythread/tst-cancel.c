
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <signal.h>
#include <execinfo.h>

#include "libmythread.h"

/* this thread does work for some time */
static void *
thread_one(void *arg)
{
    printf("[thread_one] started\n");

    for (int i = 0; ; ++i) {
        MyThreadTestCancel();
        printf("[thread_one] running thread loop... [%i]\n", i);
        usleep(500000);
    }

    printf("[thread_one] finished\n");

    return EXIT_SUCCESS;
}

int main(void)
{
    MyThread_t thread;

    printf("[main] creating thread_one...\n");
    if (MyThreadCreate(&thread, thread_one, NULL) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    sleep(3);

    printf("[main] cancelling thread_one...\n");
    if (MyThreadCancel(&thread) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    printf("[main] joining thread_one...\n");
    if (MyThreadJoin(&thread, NULL) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
