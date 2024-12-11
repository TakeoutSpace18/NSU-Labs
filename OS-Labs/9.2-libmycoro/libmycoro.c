#define _GNU_SOURCE

#include "libmycoro.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ucontext.h>

#define FORCEINLINE __attribute__((always_inline)) inline

#define STACK_SIZE (8 * 1024 * 1024) /* 8 Mb */

#define ERROR_DESCR_BUFSIZE 512
_Thread_local char __mycoro_error_descr[ERROR_DESCR_BUFSIZE];

static FORCEINLINE void
put_errno_to_error_descr(void)
{
    strncpy(__mycoro_error_descr, strerror(errno), ERROR_DESCR_BUFSIZE);
}

typedef enum state {
    ACTIVE,
    FINISHED
} state_t;

typedef struct coroutine coroutine_t;

struct coroutine
{
    void *stack;
    ucontext_t ctx;
    state_t state;

    start_routine_t routine;
    void *arg;
    void *ret;

    coroutine_t *next;
    coroutine_t *prev;
};

coroutine_t *__currently_running = NULL;

static void
coroutine_start(uintptr_t arg)
{
    coroutine_t *mcd = (coroutine_t *) arg;

    mcd->ret = mcd->routine(mcd->arg);

    mcd->state = FINISHED;
    mycoro_yield();

    /* this should never happen */
    abort();
}

int mycoro_create(mycoro_t *coro, start_routine_t func, void *arg)
{
    coroutine_t *mcd = malloc(sizeof(*mcd));
    if (!mcd) {
        put_errno_to_error_descr();
        return MYCORO_ERROR;
    }

    memset(mcd, 0, sizeof(*mcd));
    mcd->stack = malloc(STACK_SIZE);
    if (!mcd->stack) {
        free(mcd);
        put_errno_to_error_descr();
        return MYCORO_ERROR;
    }

    mcd->routine = func;
    mcd->arg = arg;
    mcd->state = ACTIVE;

    /* add new coroutine to the end of the cyclic list */
    __currently_running->prev->next = mcd;
    mcd->prev = __currently_running->prev;
    __currently_running->prev = mcd;
    mcd->next = __currently_running;

    mcd->ctx.uc_link = NULL;
    mcd->ctx.uc_stack.ss_sp = mcd->stack;
    mcd->ctx.uc_stack.ss_size = STACK_SIZE;

    getcontext(&mcd->ctx);
    makecontext(&mcd->ctx, (void (*)(void)) coroutine_start, 1, (uintptr_t) mcd);

    *coro = (mycoro_t) mcd;
    return MYCORO_OK;
}

int mycoro_init(void)
{
    coroutine_t *mcd = malloc(sizeof(*mcd));
    if (!mcd) {
        put_errno_to_error_descr();
        return MYCORO_ERROR;
    }

    memset(mcd, 0, sizeof(*mcd));
    mcd->next = mcd;
    mcd->prev = mcd;
    __currently_running = mcd;

    return MYCORO_OK;
}

static void mycoro_switch(coroutine_t *from, coroutine_t *to)
{
    __currently_running = to;
    swapcontext(&from->ctx, &to->ctx);
}

static void schedule_next(coroutine_t *current)
{
    coroutine_t *next = current->next;
    while (next->state != ACTIVE) {
        if (next->state == FINISHED) {
            free(next->stack);
        }
        next = next->next;
    }

    mycoro_switch(current, next);
    
}

void mycoro_yield(void)
{
    schedule_next(__currently_running);
}

void mycoro_join(mycoro_t coro, void **ret)
{
    coroutine_t *mcd = (coroutine_t *) coro;
    while (mcd->state != FINISHED) {
        mycoro_yield();
    }

    /* exclude from cyclic list */
    mcd->next->prev = mcd->prev;
    mcd->prev->next = mcd->next;

    if (ret != NULL) {
        *ret = mcd->ret;
    }

    free(mcd);
}
