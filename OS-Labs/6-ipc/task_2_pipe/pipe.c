#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <wait.h>

#define return_defer(code) do { status = (code); goto defer; } while (0);

void write_sequence(int write_fd)
{
    for (unsigned int i = 0; i < UINT_MAX; ++i) {
        if (write(write_fd, &i, sizeof(unsigned int)) < 0) {
            perror("write()");
            return;
        }

        usleep(2000);
        if (i % 100 == 0) {
            printf("written %i numbers\n", i);
        }
    }
}

void check_sequence(int read_fd)
{
    for (unsigned int i = 0; i < UINT_MAX; ++i) {
        unsigned int read_i;
        if (read(read_fd, &read_i, sizeof(unsigned int)) < 0) {
            perror("read()");
            return;
        }

        if (read_i != i) {
            printf("incorrect sequence at i=%i\n", i);
        }
        usleep(2000);
        if (i % 100 == 0) {
            printf("checked %i numbers\n", i);
        }
    }
}

int main()
{
    int status = EXIT_SUCCESS;

    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe()");
        return_defer(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0) {
        check_sequence(pipefd[0]);
    }
    else if (pid > 0) {
        sleep(1);
        write_sequence(pipefd[1]);
    }
    else {
        perror("fork()");
        return_defer(EXIT_FAILURE);
    }

defer:
    wait(NULL);
    return status;
}
