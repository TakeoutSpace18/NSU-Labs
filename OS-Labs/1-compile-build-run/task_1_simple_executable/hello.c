#include <stdio.h>
#include <stdlib.h>

void print_hello();

int main(void)
{
    print_hello();
    return EXIT_SUCCESS;
}

void print_hello()
{
    printf("Hello world!");
}