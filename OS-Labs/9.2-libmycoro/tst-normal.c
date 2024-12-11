#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "libmycoro.h"

#define NUM_CORO_DEFAULT 4

static inline void
create_coroutine(mycoro_t *coro, start_routine_t func, void *arg)
{
    if (mycoro_create(coro, func, arg) != MYCORO_OK) {
        fprintf(stderr, "mycoro_create(); %s\n", mycoro_strerror());
        abort();
    }
}

void *coro_func(void *arg)
{
    int64_t id = (int64_t) arg;

    printf("[coroutine %lu] A\n", id);
    mycoro_yield();
    printf("[coroutine %lu] B\n", id);
    mycoro_yield();
    printf("[coroutine %lu] C\n", id);
    mycoro_yield();
    printf("[coroutine %lu] finishing...\n", id);

    return NULL;
}

int
main(int argc, char **argv)
{
    printf("pid: %i\n", getpid());

    int num_coro;
    if (argc == 2) {
        num_coro = atoi(argv[1]);
    }
    else if (argc == 1) {
        num_coro = NUM_CORO_DEFAULT;
    }
    else {
        printf("usage: tst-normal <number of coroutines>\n");
        return EXIT_FAILURE;
    }

    if (num_coro <= 0) {
        fprintf(stderr, "num_coro should be positive\n");
        return EXIT_FAILURE;
    }

    if (mycoro_init() != MYCORO_OK) {
        fprintf(stderr, "mycoro_init() failed: %s\n", mycoro_strerror());
        abort();
    }


    mycoro_t *coros = malloc(num_coro * sizeof(mycoro_t));
    for (int64_t i = 0; i < num_coro; ++i) {
        create_coroutine(&coros[i], coro_func, (void *) i);
    }

    mycoro_yield();
    
    for (int i = 0; i < num_coro; ++i) {
        mycoro_join(coros[i], NULL);
    }

    free(coros);
    return EXIT_SUCCESS;
}

