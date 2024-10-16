#define _GNU_SOURCE

#include "libmythread.h"

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
_Thread_local char __MyThread_error_descr[ERROR_DESCR_BUFSIZE];

static FORCEINLINE void
put_errno_to_error_descr(void)
{
    strncpy(__MyThread_error_descr, strerror(errno), ERROR_DESCR_BUFSIZE);
}

static FORCEINLINE struct mythread_descriptor *
mythread_self(void)
{
    return (struct mythread_descriptor *)
        ((char *) __builtin_thread_pointer() + 1);
}

struct mythread_descriptor
{
    void *stackaddr;
    bool is_cancelled;
    bool is_running;
    void *(*start_routine)(void *);
    void *thread_arg;
    pid_t child_tid;
    void *ret;
    ucontext_t start_ctx;
};

static MtResult_t
allocate_stack(void **stackaddr, struct mythread_descriptor **mtd)
{
    /* Stack grows down and memory for thread descriptor is allocated 
     * above the stack top */

    int flags   = MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK; // | MAP_GROWSDOWN;
    int prot    = PROT_READ | PROT_WRITE;

    size_t size = STACK_SIZE + sizeof(struct mythread_descriptor);

    void *mapped = mmap(NULL, size, prot, flags, -1, 0);
    if (mapped == MAP_FAILED) {
        *stackaddr = NULL;
        *mtd = NULL;
        put_errno_to_error_descr();
        return ERROR;
    }

    *stackaddr = (char *) mapped + STACK_SIZE - 1;
    *mtd       = (struct mythread_descriptor *)((char *) mapped + STACK_SIZE);

    return OK;
}

static FORCEINLINE void
destroy_stack(void *stackaddr)
{
    void *mapped = (char *) stackaddr - STACK_SIZE + 1;
    size_t size = STACK_SIZE + sizeof(struct mythread_descriptor);
    munmap(mapped, size);
}

static int
thread_start(void *arg)
{
    struct mythread_descriptor *mtd = (struct mythread_descriptor *) arg;
    
    mtd->is_running = true;
    mtd->is_cancelled  = false;
    mtd->ret = NULL;

    if (getcontext(&mtd->start_ctx) == -1) {
        put_errno_to_error_descr();
        return EXIT_FAILURE;
    }
      
    if (!mtd->is_cancelled)
        mtd->ret = mtd->start_routine(mtd->thread_arg);

    mtd->is_running = false;
    return EXIT_SUCCESS;
}

MtResult_t
MyThreadCreate(MyThread_t *thread, void *(*start_routine)(void *), void *arg)
{
    void                        *stacktop  = NULL;
    struct mythread_descriptor  *mtd        = NULL;

    MtResult_t status = allocate_stack(&stacktop, &mtd);
    if (status != OK)
        return status;

    mtd->stackaddr     = stacktop;
    mtd->start_routine = start_routine;
    mtd->thread_arg    = arg;

    int flags = 0;
    flags |= CLONE_VM; /* Common address space */
    flags |= CLONE_FS; /* Common filesystem state */
    flags |= CLONE_FILES; /* Common file descriptors */
    flags |= CLONE_SIGHAND; /* Common signal handlers */
    flags |= CLONE_THREAD; /* To share same PID */
    flags |= CLONE_SETTLS; /* Set thread local storage */

    /* FUTEX_WAKE will be called on child_tid upon thread termination */
    /* used for joining the thread */
    flags |= CLONE_CHILD_CLEARTID;


    int tid = clone(thread_start, stacktop, flags, mtd,
                    NULL, mtd, &mtd->child_tid);
    if (tid == -1) {
        put_errno_to_error_descr();
        destroy_stack(stacktop);
        return ERROR;
    }

    *thread = (MyThread_t) mtd;
    return OK;
}

MyThread_t
MyThreadSelf(void)
{
    return (MyThread_t) mythread_self();
}

static int
futex(int* uaddr, int futex_op, int val, const struct timespec* timeout,
      int* uaddr2, int val3)
{
  return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

static void
wait_on_futex_value(int* futex_addr, int val)
{
    while (1) {
        int futex_rc = futex(futex_addr, FUTEX_WAIT, val, NULL, NULL, 0);
        if (futex_rc == -1) {
            if (errno != EAGAIN) {
                perror("futex");
                exit(1);
            }
        } else if (futex_rc == 0) {
            if (*futex_addr == val)
                return;
        } else {
            abort();
        }
    }
}

MtResult_t
MyThreadJoin(MyThread_t *thread, void **ret)
{
    struct mythread_descriptor *mtd = (struct mythread_descriptor *) *thread;
    
    if (mtd->is_running)
        wait_on_futex_value(&mtd->child_tid, 0);

    if (ret)
        *ret = mtd->ret;

    destroy_stack(mtd->stackaddr);
    return OK;
}

MtResult_t
MyThreadCancel(MyThread_t *thread)
{
    struct mythread_descriptor *mtd = (struct mythread_descriptor *) *thread;
    mtd->is_cancelled = true;

    return OK;
}

void
MyThreadTestCancel(void)
{
    struct mythread_descriptor *mtd = mythread_self();
    if (mtd->is_cancelled) {
        if (setcontext(&mtd->start_ctx) == -1) {
            fputs(strerror(errno), stderr);
            abort();
        }
    }
}
