#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    printf("pid: %i\n", getpid());
    sleep(1);
    execv(argv[0], argv);

    printf("Hello world!\n");
    return EXIT_SUCCESS;
}
