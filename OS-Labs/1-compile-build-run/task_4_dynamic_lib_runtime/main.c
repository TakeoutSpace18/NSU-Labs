#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "hello_dynamic_lib.h"

int main(void)
{
    void* shared_object = dlopen("libhello.so", RTLD_LAZY);
    if (!shared_object)
    {
        printf("dlopen() failed: %s\n", dlerror());
        return EXIT_FAILURE;
    }


    print_hello_fn hello_fn = dlsym(shared_object, "print_hello");
    if (!hello_fn)
    {
        printf("dlsym() failed: %s\n", dlerror());
    }

    hello_fn();

    return EXIT_SUCCESS;
}