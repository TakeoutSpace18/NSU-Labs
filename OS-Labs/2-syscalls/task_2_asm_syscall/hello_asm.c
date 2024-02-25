#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/syscall.h>

int print_hello();

int main(void)
{
    return print_hello();
}

int print_hello()
{
    uint64_t syscall_number = SYS_write;
    uint64_t fd = STDOUT_FILENO;
    const char buf[] = "Hello, world!\n";
    size_t count = sizeof(buf) - 1;

    ssize_t return_value;

    asm volatile (
        "movq %1, %%rax\n\t"
        "movq %2, %%rdi\n\t"
        "movq %3, %%rsi\n\t"
        "movq %4, %%rdx\n\t"
        "syscall\n\t"
        "movq %%rax, %0"
        : "=r"(return_value)
        : "r"(syscall_number), "r"(fd), "r"(buf), "r"(count)
        : "%rax", "%rdi", "%rsi", "%rdx"
    );

    perror("SYS_write");

    return EXIT_SUCCESS;
}
