#define _GNU_SOURCE

#include "libmythread.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <sched.h>
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

#define FORCEINLINE __attribute__((always_inline)) inline

/* Maybe allocate less in case of using MAP_GROWSDOWN ?*/
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
    return (struct mythread_descriptor *) __builtin_thread_pointer();
}

struct mythread_descriptor
{
    void *stackaddr;
    bool is_cancelled;
    void *(*start_routine)(void *);
    void *thread_arg;
};

static MtResult_t
allocate_stack(void **stackaddr, struct mythread_descriptor **mtd)
{
    /* Stack grows down and memory for thread descriptor is allocated 
     * above the stack top */

    int flags   = MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK | MAP_GROWSDOWN;
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
destroy_stack(void **stackaddr)
{
    void *mapped = (char *) *stackaddr - STACK_SIZE + 1;
    size_t size = STACK_SIZE + sizeof(struct mythread_descriptor);
    munmap(mapped, size);
    *stackaddr = NULL;
}

static int
thread_start(void *arg)
{
    struct mythread_descriptor *mtd = (struct mythread_descriptor *) arg;
    
    /* Put thread descriptor pointer to FS register */
    if (syscall(SYS_arch_prctl, ARCH_SET_FS, mtd) != 0) {
        put_errno_to_error_descr();
        return EXIT_FAILURE;
    }

    void *ret = mtd->start_routine(mtd->thread_arg);

    return EXIT_SUCCESS;
}

MtResult_t
MyThreadCreate(MyThread_t *thread, void *(*start_routine)(void *), void *arg)
{
    int flags = 0;
    flags |= CLONE_VM; /* Common address space */
    flags |= CLONE_FS; /* Common filesystem state */
    flags |= CLONE_FILES; /* Common file descriptors */
    flags |= CLONE_SIGHAND; /* Common signal handlers */
    flags |= CLONE_THREAD; /* To share same PID */
    // flags |= CLONE_PARENT_SETTID;
    /* flags |= CLONE_CHILD_CLEARTID; */
    /* flags |= CLONE_SYSVSEM; */

    void                        *stackaddr  = NULL;
    struct mythread_descriptor  *mtd        = NULL;

    MtResult_t status = allocate_stack(&stackaddr, &mtd);
    if (status != OK)
        return status;

    mtd->stackaddr    = stackaddr;
    mtd->is_cancelled = false;
    mtd->start_routine = start_routine;
    mtd->thread_arg = arg;

    int tid = clone(thread_start, stackaddr, flags, mtd);
    if (tid == -1) {
        put_errno_to_error_descr();
        destroy_stack(&stackaddr);
        return ERROR;
    }

    printf("%zi", sizeof(struct mythread_descriptor));
    printf("mtd: %p\n", (void *) mtd);
    *thread = (MyThread_t) mtd;
    return OK;
}

MyThread_t
MyThreadSelf(void)
{
    return (MyThread_t) mythread_self();
}

MtResult_t
MyThreadJoin(MyThread_t *thread);

MtResult_t
MyThreadDetach(MyThread_t *thread);

MtResult_t
MyThreadCancel(MyThread_t *thread);

void
MyThreadTestCancel(void)
{
    struct mythread_descriptor *mtd = mythread_self();
    printf("mtd: %p\n", (void *) mtd);
}
