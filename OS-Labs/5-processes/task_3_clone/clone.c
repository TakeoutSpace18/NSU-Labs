#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 2) /* 2 kB*/

void hello_recursion(int depth)
{
    if (depth == 0) {
        return;
    }

    char hello[]= "hello world";

    puts(hello);
    hello_recursion(depth - 1);
}

int new_process(void *arg)
{
    hello_recursion(10);
    return EXIT_SUCCESS;
}

int main()
{
    int fd = open("stack_dump.bin", O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if (fd < 0) {
        perror("open()");
        return EXIT_FAILURE;
    }

    if (ftruncate(fd, STACK_SIZE) < 0) {
        perror("truncate()");
        return EXIT_FAILURE;
    }

    void *stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (stack == MAP_FAILED) {
        perror("mmap()");
        return EXIT_FAILURE;
    }

    void *stack_top = stack + STACK_SIZE;
    printf("stack_top: %p\n", stack_top);

    pid_t pid = clone(new_process, stack_top, CLONE_VM | SIGCHLD, NULL);

    wait(NULL);
    close(fd);
    munmap(stack, STACK_SIZE);
    return EXIT_SUCCESS;
}
