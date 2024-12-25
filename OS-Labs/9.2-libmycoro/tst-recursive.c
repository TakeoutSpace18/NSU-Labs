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

typedef struct {
    uint64_t target_depth;
    uint64_t current_depth;
} coro_arg_t;

static void *coro_func(void *arg)
{
    coro_arg_t *coro_arg = (coro_arg_t *) arg;

    if (coro_arg->current_depth < coro_arg->target_depth) {
        mycoro_t new_coro;
        coro_arg_t new_arg = {
            .target_depth = coro_arg->target_depth,
            .current_depth = coro_arg->current_depth + 1
        };
        create_coroutine(&new_coro, coro_func, &new_arg);
        printf("[%lu] created new coroutine\n", coro_arg->current_depth);

        uint64_t ret;
        mycoro_join(new_coro, (void **) &ret);
        
        printf("[%lu] coroutine returned %lu\n", coro_arg->current_depth, ret);
    }

    return (void *) coro_arg->current_depth;
}

int main(int argc, char **argv)
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
        printf("usage: tst-recursive <number of coroutines>\n");
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

    mycoro_t new_coro;
    coro_arg_t new_arg = {
        .target_depth = num_coro,
        .current_depth = 0
    };

    create_coroutine(&new_coro, coro_func, &new_arg);
    mycoro_join(new_coro, NULL);
    
    return EXIT_SUCCESS;
}
