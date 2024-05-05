#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int grandparent();
int parent();
int child();

int grandparent()
{
    printf("GRANDPARENT: pid: %i\n", getpid());

    pid_t pid = fork();

    if (pid == 0) {
        return parent();
    }
    if (pid > 0) {
        sleep(60);
        return EXIT_SUCCESS;
    }

    perror("fork()");
    return EXIT_FAILURE;
}

int parent()
{
    printf("PARENT: pid: %i\n", getpid());

    pid_t pid = fork();

    if (pid == 0) {
        return child();
    }
    if (pid > 0) {
        // become zombie
        return EXIT_SUCCESS;
    }

    perror("fork()");
    return EXIT_FAILURE;
}

int child()
{
    printf("CHILD: pid: %i\n", getpid());
    sleep(60);
    return EXIT_SUCCESS;
}

int main()
{
    return grandparent();
}
