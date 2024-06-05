#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include <signal.h>
#include <sys/resource.h>
#include <sys/mman.h>

#define ALLOCATION_DELAY_NS 500000

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static bool sigint_recieved = false;

void sigsegv_handler(int sig)
{
    struct rlimit lm;
    if (getrlimit(RLIMIT_STACK, &lm) < 0) {
        handle_error("getrlimit");
    }

    printf("SIGSEGV recieved: stack soft limit: %lu, stack hard limit: %lu\n", lm.rlim_cur, lm.rlim_max);

    lm.rlim_cur *= 2;
    if (setrlimit(RLIMIT_STACK, &lm) < 0) {
        handle_error("setrlimit");
    }
}

void sigint_handler(int sig)
{
    printf("SIGINT recieved\n");
    sigint_recieved = true;
}

void stack_overflow()
{
    int step_size = 4096 * 32;

    char arr[step_size];
    static int total_allocated_on_stack = 0;
    total_allocated_on_stack += step_size;

    printf("allocated %i bytes on stack\n", total_allocated_on_stack);
    usleep(ALLOCATION_DELAY_NS);

    if (!sigint_recieved) {
        stack_overflow();
    }
    else {
        sigint_recieved = false;
    }
}

#define MAX_ALLOCATIONS 4096
void heap_allocation()
{
    const int step_size = 1024*128;
    void *ptrs[MAX_ALLOCATIONS];

    static int total_allocated_on_heap = 0;


    int alloc_count = 0;
    while (alloc_count < MAX_ALLOCATIONS && !sigint_recieved) {
        ptrs[alloc_count++] = malloc(step_size);
        total_allocated_on_heap += step_size;
        printf("allocated %i bytes on heap\n", total_allocated_on_heap);
        usleep(ALLOCATION_DELAY_NS);

    }

    sigint_recieved = false;

    malloc_info(0, stdout);

    for (int i = 0; i < alloc_count; ++i) {
        free(ptrs[i]);
    }
}

void setup_sigsegv_handler(void)
{
    stack_t altstack = {0};
    altstack.ss_sp = malloc(SIGSTKSZ);
    if (altstack.ss_sp == NULL) {
        handle_error("malloc");
    }
    altstack.ss_size = SIGSTKSZ;
    altstack.ss_flags = 0;

    if (sigaltstack(&altstack, NULL) < 0) {
        handle_error("sigaltstack");
    }

    struct sigaction sa = {0};
    sa.sa_flags |= SA_ONSTACK;
    sa.sa_handler = sigsegv_handler;

    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        handle_error("sigaction");
    }
}

void setup_sigint_handler(void)
{
    struct sigaction sa = {0};
    sa.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &sa, NULL) < 0) {
        handle_error("sigaction");
    }
}


int main(int argc, char** argv)
{
    printf("pid: %i\n", getpid());
    sleep(10);

    setup_sigsegv_handler();
    setup_sigint_handler();

    stack_overflow();
    heap_allocation();

    return EXIT_SUCCESS;
}
