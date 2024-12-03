#define _GNU_SOURCE

#include "libmycoro.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <linux/sched.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <asm/prctl.h>
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

struct mycoro_descriptor
{
    void *stackaddr;
    start_routine_t start_routine;
    void *coroutine_arg;
    ucontext_t ctx;
};

static int
allocate_stack(void **stackaddr, struct mycoro_descriptor **mcd)
{
    /* Stack grows down and memory for coroutine descriptor is allocated 
     * above the stack top */

    int flags   = MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK; // | MAP_GROWSDOWN;
    int prot    = PROT_READ | PROT_WRITE;

    size_t size = STACK_SIZE + sizeof(struct mycoro_descriptor);

    void *mapped = mmap(NULL, size, prot, flags, -1, 0);
    if (mapped == MAP_FAILED) {
        *stackaddr = NULL;
        *mcd = NULL;
        put_errno_to_error_descr();
        return MYCORO_ERROR;
    }

    *stackaddr = (char *) mapped + STACK_SIZE - 1;
    *mcd       = (struct mycoro_descriptor *)((char *) mapped + STACK_SIZE);

    return MYCORO_OK;
}

static void *stack_bottom(void *stacktop) {
    return (char *) stacktop - STACK_SIZE + 1;
}

static FORCEINLINE void
destroy_stack(void *stackaddr)
{
    void *mapped = (char *) stackaddr - STACK_SIZE + 1;
    size_t size = STACK_SIZE + sizeof(struct mycoro_descriptor);
    munmap(mapped, size);
}

static void
coroutine_start(uintptr_t arg)
{
    struct mycoro_descriptor *mcd = (struct mycoro_descriptor *) arg;

    mcd->start_routine(mcd->coroutine_arg);

    fprintf(stderr, "FATAL: coroutine finished execution\n");
    abort();
}

int mycoro_create(mycoro_t *coro, start_routine_t func, void *arg)
{
    void                     *stacktop = NULL;
    struct mycoro_descriptor *mcd      = NULL;

    int status = allocate_stack(&stacktop, &mcd);
    if (status != MYCORO_OK)
        return status;

    memset(mcd, 0, sizeof(*mcd));
    mcd->stackaddr     = stacktop;
    mcd->start_routine = func;
    mcd->coroutine_arg = arg;

    mcd->ctx.uc_link = NULL;
    mcd->ctx.uc_stack.ss_sp = stack_bottom(stacktop);
    mcd->ctx.uc_stack.ss_size = STACK_SIZE;

    getcontext(&mcd->ctx);
    makecontext(&mcd->ctx, (void (*)(void)) coroutine_start, 1, (uintptr_t) mcd);

    *coro = (mycoro_t) mcd;
    return MYCORO_OK;
}

int mycoro_init(mycoro_t *main)
{
    struct mycoro_descriptor *mcd = malloc(sizeof(*mcd));
    if (!mcd) {
        put_errno_to_error_descr();
        return MYCORO_ERROR;
    }

    memset(mcd, 0, sizeof(*mcd));
    *main = (mycoro_t) mcd;

    return MYCORO_OK;
}

void mycoro_destroy(mycoro_t coro)
{
    struct mycoro_descriptor *mcd = (struct mycoro_descriptor *) coro;

    /* NULL stackaddr means descriptor is from *main* coroutine */
    if (mcd->stackaddr == NULL) {
        free(mcd);
    }
    else {
        destroy_stack(mcd->stackaddr);
    }
}

void mycoro_switch(mycoro_t from, mycoro_t to)
{
    struct mycoro_descriptor *mcd_from = (struct mycoro_descriptor *) from;
    struct mycoro_descriptor *mcd_to = (struct mycoro_descriptor *) to;

    swapcontext(&mcd_from->ctx, &mcd_to->ctx);
}
