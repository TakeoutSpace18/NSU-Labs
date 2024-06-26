#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global_var = 4;

int main()
{
    printf("PARENT: pid: %i\n", getpid());

    int local_var = 5;

    printf("PARENT: local_var: %i, addr: %p\n", local_var, &local_var);
    printf("PARENT: global_var: %i, addr: %p\n", global_var, &global_var);

    pid_t pid = fork();

    if (pid == 0) {
        printf("CHILD: pid: %i, ppid %i\n", getpid(), getppid());
        printf("CLILD local_var: %i, addr: %p\n", local_var, &local_var);
        printf("CHILD: global_var: %i, addr: %p\n", global_var, &global_var);

        getc(stdin);

        printf("CHILD: Altering variables...\n");
        local_var = 11;
        global_var = 10;

        printf("CLILD: local_var: %i, addr: %p\n", local_var, &local_var);
        printf("CHILD: global_var: %i, addr: %p\n", global_var, &global_var);

        sleep(10);

        printf("CHILD: Exiting...\n");
        exit(5);
    }
    else if (pid > 0) {
        getc(stdin);

        printf("PARENT: local_var: %i, addr: %p\n", local_var, &local_var);
        printf("PARENT: global_var: %i, addr: %p\n", global_var, &global_var);

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
