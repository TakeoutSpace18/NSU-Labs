#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

int global_uninit;          // located in BSS section
int global_init = 4;        // located in initialized data section
const int global_const = 5;

void print_addresses()
{
    int local = 1;
    static int local_static = 2;
    const int local_const = 3;

    printf("local: %p\n", &local);
    printf("static local: %p\n", &local_static);
    printf("const local: %p\n", &local_const);
    printf("\n");

    printf("global initialized: %p\n", &global_init);
    printf("global uninitialized: %p\n", &global_uninit);
    printf("const global: %p\n", &global_const);

}

int *return_local_var()
{
    int var = 5;
    return &var;
}

void heap_allocation()
{
    if (mallopt(M_CHECK_ACTION, 3) != 1) {
        fprintf(stderr, "mallopt() failed");
        exit(EXIT_FAILURE);
    }

    char* buf1 = malloc(100);
    strcpy(buf1, "hello world");
    printf("buf1: %s", buf1);
    free(buf1);

    char* buf2 = malloc(100);
    strcpy(buf2, "hello world");
    printf("buf2: %s", buf2);

    *(size_t*)(buf2 + 24) = 100;
    *(size_t*)(buf2 + 28) = 100;
    free(buf2 + 32);
    printf("buf2 after free in the middle: %s", buf2);
}

void env_variable(void)
{
    char *var = getenv("MY_VAR");
    if (!var) {
        fprintf(stderr, "env var is not set!");
        exit(EXIT_FAILURE);
    }
    printf("MY_VAR: %s\n", var);
    setenv("MY_VAR", "changed", 1);
    var = getenv("MY_VAR");
    printf("MY_VAR: %s\n", var);
}

int main()
{
    printf("pid: %i\n", getpid());

    print_addresses();

    printf("\n");
    int* ptr = return_local_var();
    printf("returned addr of local variable: %p\n", ptr);

    printf("\n");
    heap_allocation();
    printf("\n");

    // env_variable();

    getc(stdin);
    return EXIT_SUCCESS;
}
