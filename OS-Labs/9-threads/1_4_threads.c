#define _GNU_SOURCE

#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

#define STRSIZE 256

void cleanup(char *p) {
    printf("%p\n", p);
}

static void*
thread_main(void *arg)
{
    /* it is unsafe to use async cancellation type with malloc */
    /* async cancel is implemented using a signal (NPTL)*/

    // int canceltype = PTHREAD_CANCEL_DEFERRED;
    int canceltype = PTHREAD_CANCEL_ASYNCHRONOUS;
    if (pthread_setcanceltype(canceltype, NULL) != 0) {
        perror("pthread_setcanceltype()");
        pthread_exit(EXIT_FAILURE);
    }

    char *str = NULL;
    pthread_cleanup_push(free, str);
    str = malloc(STRSIZE);

    for (int i = 0; true; i++) {
        printf("thread is working (%i)\n", i);

        // snprintf(str, STRSIZE, "thread is working (%i)", i);
        // puts(str);

        usleep(50000);
    }

    pthread_cleanup_pop(true);
    return NULL;
}

int
main() 
{
    printf("pid: %i\n", getpid());
    getc(stdin);

    printf("creating thread...\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t thread;
    if (pthread_create(&thread, &attr, thread_main, NULL) != 0) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    sleep(2);

    printf("cancelling thread...\n");
    if (pthread_cancel(thread) != 0) {
        perror("pthread_cancel()");
        return EXIT_FAILURE;
    }

    if (pthread_join(thread, NULL) != 0) {
        perror("pthread_join()");
        return EXIT_FAILURE;
    }

    printf("finishing...\n");
    return EXIT_SUCCESS;
}
