#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int global_var = 4;

int main()
{
    printf("PARENT: pid: %i\n", getpid());

    int local_var = 5;
    int *heap_var = malloc(sizeof(int));
    *heap_var = 6;

    char *mmaped_var = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    memset(mmaped_var, 1, sysconf(_SC_PAGESIZE));


    printf("PARENT: local_var: %i, addr: %p\n", local_var, &local_var);
    printf("PARENT: global_var: %i, addr: %p\n", global_var, &global_var);
    printf("PARENT: heap_var: %i, addr: %p\n", *heap_var, heap_var);
    printf("PARENT: mmapped_var: %i, addr: %p\n", *mmaped_var, mmaped_var);

    getc(stdin);
    printf("PARENT: forking...\n");

    pid_t pid = fork();
    getc(stdin);

    if (pid == 0) {
        printf("CHILD: pid: %i, ppid %i\n", getpid(), getppid());

        getc(stdin);

        printf("CHILD: Altering variables...\n");
        local_var = 11;
        global_var = 10;
        *heap_var = 12;
        memset(mmaped_var, 5, sysconf(_SC_PAGESIZE));

        printf("CHILD: local_var: %i, addr: %p\n", local_var, &local_var);
        printf("CHILD: global_var: %i, addr: %p\n", global_var, &global_var);
        printf("CHILD: heap_var: %i, addr: %p\n", *heap_var, heap_var);
        printf("PARENT: mmapped_var: %i, addr: %p\n", *mmaped_var, mmaped_var);

        getc(stdin);

        printf("CHILD: Exiting...\n");
        exit(5);
    }
    else if (pid > 0) {
        getc(stdin);

        printf("PARENT: waiting for child...\n");
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("PARENT: child process terminated with code %i\n", WEXITSTATUS(status));
        }
        else if (WIFSIGNALED(status)) {
            printf("PARENT: child terminated recieving a signal %i\n", WTERMSIG(status));
        }
        else if (WIFSTOPPED(status)) {
            printf("PARENT: child stopped recieving a signal %i\n", WSTOPSIG(status));
        }
    }
    else {
        perror("fork()");
        return EXIT_FAILURE;
    }

    getc(stdin);
    return EXIT_SUCCESS;
}