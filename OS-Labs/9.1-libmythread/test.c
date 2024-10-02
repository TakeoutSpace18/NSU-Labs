#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libmythread.h"

static void *
thread_main(void *arg)
{
    printf("thread main()\n");

    for (;;) {
        printf("running thread loop...\n");
        MyThreadTestCancel();
        sleep(1);
    }

    return EXIT_SUCCESS;
}

int main(void)
{
    MyThread_t thread;

    if (MyThreadCreate(&thread, thread_main, NULL) != OK) {
        fprintf(stderr, "%s\n", MyThreadErrorDescr());
        return EXIT_FAILURE;
    }

    printf("created thread\n");

    sleep(1000);

    return EXIT_SUCCESS;
}
