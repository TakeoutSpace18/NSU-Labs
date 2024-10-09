#define _GNU_SOURCE

#include <stdlib.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

void *get_meaning_of_life(void *arg)
{
    printf("Computing meaning of life...\n");
    usleep(500000);
    return (void *) 42;
}

void *get_string_pointer(void *arg)
{
    usleep(500000);
    char *s = "hello world";
    return s;
}

void *tid_printer(void *arg)
{
    // pthread_detach(pthread_self());
    // usleep(500000);
    printf("hello from thread [tid=%i]\n", gettid());
    return NULL;
}

int main() 
{
    printf("pid: %i\n", getpid());
    getc(stdin);

    pthread_t thread;

    if (pthread_create(&thread, NULL, get_meaning_of_life, NULL) != 0) {
        perror("pthread_create()");
        return EXIT_FAILURE;
    }

    void *retval;
    if (pthread_join(thread, &retval) != 0) {
        perror("pthread_join()");
        return EXIT_FAILURE;
    }

    printf("thread returned: %li\n", (int64_t) retval);

    if (pthread_create(&thread, NULL, get_string_pointer, NULL) != 0) {
        perror("pthread_create()");
        return EXIT_FAILURE;
    }

    if (pthread_join(thread, &retval) != 0) {
        perror("pthread_join()");
        return EXIT_FAILURE;
    }

    printf("thread returned: %s\n", (char *) retval);

    for (size_t i = 0;;i++) {
        printf("creating thread %zi...\n", i);

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        if (pthread_create(&thread, &attr, tid_printer, NULL) != 0) {
            perror("pthread_create()");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
