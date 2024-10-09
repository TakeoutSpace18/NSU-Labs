#define _GNU_SOURCE

#include <sys/cdefs.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

// #define USE_MALLOC

typedef struct thread_arg
{
    int integer;
    char *char_ptr;
}
thread_arg_t;

static void __attribute_maybe_unused__
arg_cleanup(void *arg)
{
    free(arg);
}

static void*
thread_two(void *arg) /* args printer */
{
    sleep(1);
    fgetc(stdin);

#ifdef USE_MALLOC
    pthread_cleanup_push(arg_cleanup, arg);
#endif

    printf("thread_two printing args...\n");

    thread_arg_t *data = (thread_arg_t *) arg;
    printf("int value: %i\n", data->integer);
    printf("char* value: %s\n", data->char_ptr);

#ifdef USE_MALLOC
    pthread_cleanup_pop(true);
#endif
    return NULL;
}


static void *
thread_one(void *argp)
{
#ifdef USE_MALLOC
    thread_arg_t *arg = malloc(sizeof(thread_arg_t));
    *arg = (thread_arg_t) {
        .integer = 42,
        .char_ptr = "some string"
    };

#else
    thread_arg_t arg_storage = {
        .integer = 42,
        .char_ptr = "some string",
    };

    printf("arg_storage address: %p\n", &arg_storage);

    thread_arg_t *arg = &arg_storage;
#endif

    fgetc(stdin);

    printf("creating thread two...\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    if (pthread_create(&thread, &attr, thread_two, arg) != 0) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    printf("thread_one finished\n");
    return EXIT_SUCCESS;
}

int
main() 
{
    printf("pid: %i\n", getpid());
    getc(stdin);

    printf("creating thread one...\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    if (pthread_create(&thread, &attr, thread_one, NULL) != 0) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }

    sleep(120);
    return EXIT_SUCCESS;
}
