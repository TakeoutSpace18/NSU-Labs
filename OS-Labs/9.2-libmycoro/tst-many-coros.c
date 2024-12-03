#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "libmycoro.h"

#define NUM_CORO_DEFAULT 4

mycoro_t main_context;

typedef struct {
    mycoro_t ctx;
    int number;
} coroutine_t;

static void
coroutine_funcB(coroutine_t *self, useconds_t sleep_time)
{
    printf("coro #%i: coroutine_funcB()\n", self->number);
    usleep(sleep_time);
    mycoro_switch(self->ctx, main_context);
}

static void
coroutine_funcA(coroutine_t *self, useconds_t sleep_time)
{
    printf("coro #%i: coroutine_funcA()\n", self->number);
    usleep(sleep_time);
    mycoro_switch(self->ctx, main_context);

    coroutine_funcB(self, sleep_time);
}

static void
coroutine_main(void *arg)
{
    coroutine_t *self = (coroutine_t*) arg; 
    useconds_t sleep_time = 400000;
    int iter = 0;
    for (;;) {
        printf("coro #%i; iter #%i: begin\n", self->number, iter);
        usleep(sleep_time);
        mycoro_switch(self->ctx, main_context);

        coroutine_funcA(self, sleep_time);

        printf("coro #%i; iter #%i: end\n", self->number, iter);
        usleep(sleep_time);
        mycoro_switch(self->ctx, main_context);

        ++iter;
    }
}

static void
create_coroutine(coroutine_t *c, void (*start_routine)(void *), void *arg)
{
    if (mycoro_create(&c->ctx, start_routine, arg) != MYCORO_OK) {
        fprintf(stderr, "mycoro_create() failed: %s\n", mycoro_strerror());
        abort();
    }
}

static void
destroy_coroutine(coroutine_t *c)
{
    mycoro_destroy(c->ctx);
}

static coroutine_t*
create_coro_array(int num_coro)
{
    coroutine_t *coros = malloc(num_coro * sizeof(coroutine_t));
    if (!coros) {
        fprintf(stderr, "out of memory\n");
        abort();
    }

    for (int i = 0; i < num_coro; ++i) {
        coros[i].number = i;
        create_coroutine(&coros[i], &coroutine_main, &coros[i]);
    }

    return coros;
}

static void
destroy_coro_array(coroutine_t **array, int num_coro)
{
    for (int i = 0; i < num_coro; ++i){
        destroy_coroutine(&(*array)[i]);
    }

    free(*array);
    *array = NULL;
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
        printf("usage: corodemo <number of coroutines>\n");
    }

    if (num_coro <= 0) {
        fprintf(stderr, "num_coro should be positive\n");
    }

    if (mycoro_init(&main_context) != MYCORO_OK) {
        fprintf(stderr, "mycoro_init() failed: %s\n", mycoro_strerror());
        abort();
    }

    coroutine_t *coros = create_coro_array(num_coro);

    int cur_coro = 0;
    while (true) {
        mycoro_switch(main_context, coros[cur_coro].ctx);
        cur_coro = (cur_coro + 1) % num_coro;
    }

    destroy_coro_array(&coros, num_coro);
    return EXIT_SUCCESS;
}
