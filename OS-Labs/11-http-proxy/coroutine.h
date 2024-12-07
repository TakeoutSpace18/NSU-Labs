#ifndef COROUTINE_H
#define COROUTINE_H

#include <coro.h>
#include <string.h>

#include "c.h"
#include "log.h"

typedef void (*coroutine_func_t)(void *);

typedef struct coroutine {
    struct coro_context ctx;
    struct coro_stack stack;
} coroutine_t;

static inline int coroutine_create(coroutine_t *c, coroutine_func_t func, void *arg)
{
    if (!coro_stack_alloc(&c->stack, 0)) {
        log_error("Failed to allocate coroutine stack");
        return ERROR;
    }

    coro_create(&c->ctx, func, arg, c->stack.sptr, c->stack.ssze);

    return OK;
}

static inline void coroutine_create_initial(coroutine_t *c)
{
    memset(&c->stack, 0, sizeof(c->stack));
    coro_create(&c->ctx, NULL, NULL, NULL, 0);
}

static inline void coroutine_destroy(coroutine_t *c)
{
    coro_stack_free(&c->stack);
}

static inline void coroutine_switch(coroutine_t *prev, coroutine_t *next)
{
    coro_transfer(&prev->ctx, &next->ctx);
}

#endif // !COROUTINE_H
