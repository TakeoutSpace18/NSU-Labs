#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <sys/mman.h>

#define STEP_DELAY 10

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void sigsegv_handler(int sig, siginfo_t *info, void *unused)
{
    printf("SIGSEGV recieved, addr: %p\n", info->si_addr);
    getc(stdin);
    exit(EXIT_FAILURE);
}

void mmap_allocation(void)
{
    long int pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize < 0) {
        handle_error("sysconf");
    }

    void* addr = mmap(NULL, pagesize * 10, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (!addr) {
        handle_error("mmap");
    }
    printf("Allocated 10 pages with mmap at %p, page size: %li\n", addr, pagesize);

    getc(stdin);
    if (munmap(addr + 3 * pagesize, 3 * pagesize) < 0) {
        handle_error("munmap");
    }
    printf("Detached pages 4-6\n");

    getc(stdin);
    if (mprotect(addr + pagesize, pagesize, PROT_WRITE) < 0) {
        handle_error("mprotect");
    }
    printf("Changed second page protection to writeonly\n");

    getc(stdin);

    printf("Reading from read protected page... (%p) %c\n", addr + pagesize, *(char*)(addr + pagesize));

    getc(stdin);

    if (mprotect(addr + 2 * pagesize, pagesize, PROT_READ) < 0) {
        handle_error("mprotect");
    }
    printf("Changed third page protection to readonly\n");

    getc(stdin);

    printf("Writing to write protected page... (%p)\n", addr + 2 * pagesize);
    *(char*)(addr + 2 * pagesize) = 'A';
}

void setup_sigsegv_handler(void)
{
    struct sigaction sa = {0};
    sa.sa_sigaction = sigsegv_handler;
    sa.sa_flags |= SA_SIGINFO;

    if (sigaction(SIGSEGV, &sa, NULL) < 0) {
        handle_error("sigaction");
    }
}

int main(int argc, char** argv)
{
    printf("pid: %i\n", getpid());
    getc(stdin);

    setup_sigsegv_handler();

    mmap_allocation();

    return EXIT_SUCCESS;
}
