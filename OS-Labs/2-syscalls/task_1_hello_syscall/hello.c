#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

void print_hello();

int main(void)
{
    print_hello();
    return EXIT_SUCCESS;
}

void print_hello()
{
    const char msg[] = "Hello world!\n";

    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
    syscall(SYS_write, STDOUT_FILENO, msg, sizeof(msg) - 1);
    printf("Hello world!\n");
}