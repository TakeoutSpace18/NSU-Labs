#include <sys/cdefs.h>
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

#define USE_MALLOC

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
args_printer(void *arg)
{
#ifdef USE_MALLOC
    pthread_cleanup_push(arg_cleanup, arg);
#endif

    usleep(500000);
    thread_arg_t *data = (thread_arg_t *) arg;
    printf("int value: %i\n", data->integer);
    printf("char* value: %s\n", data->char_ptr);

#ifdef USE_MALLOC
    pthread_cleanup_pop(true);
#endif
    return NULL;
}


static void
create_thread(void)
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

    thread_arg_t *arg = &arg_storage;
#endif

    printf("creating thread...\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_t thread;
    if (pthread_create(&thread, &attr, args_printer, arg) != 0) {
        perror("pthread_create()");
        exit(EXIT_FAILURE);
    }
}

int
main() 
{
    printf("pid: %i\n", getpid());
    getc(stdin);

    create_thread();

    pthread_exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}
