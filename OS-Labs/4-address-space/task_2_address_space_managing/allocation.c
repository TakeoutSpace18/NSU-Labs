#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

size_t total_allocated_on_stack = 0;

void stack_overflow()
{
    char arr[4096];
    total_allocated_on_stack += 4096;
    printf("allocated %llu bytes on stack\n", total_allocated_on_stack);
    usleep(500000);
    stack_overflow();
}

int main(int argc, char** argv)
{
    printf("pid: %i\n", getpid());
    sleep(15);

    stack_overflow();

    return EXIT_SUCCESS;
}
