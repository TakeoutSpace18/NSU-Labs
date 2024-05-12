#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <wait.h>

#define return_defer(code) do { status = (code); goto defer; } while (0);

sem_t *semaphore = NULL;

void write_sequence(unsigned int *array, size_t size)
{
    for (unsigned int i = 0; i < UINT_MAX; ++i) {
        array[i % size] = i;
        sem_post(semaphore);
        usleep(2000);
        if (i % 100 == 0) {
            printf("written %i numbers\n", i);
        }
    }
}

void check_sequence(const unsigned int *array, size_t size)
{
    for (unsigned int i = 0; i < UINT_MAX; ++i) {
        sem_wait(semaphore);
        if (array[i % size] != i) {
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

    size_t shmem_size = sysconf(_SC_PAGESIZE);

    void *shmem = mmap(NULL, shmem_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shmem == MAP_FAILED) {
        perror("mmap()");
        return_defer(EXIT_FAILURE);
    }

    semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (semaphore == MAP_FAILED) {
        perror("mmap()");
        return_defer(EXIT_FAILURE);
    }

    if (sem_init(semaphore, 1, 0) < 0) {
        perror("sem_init()");
        return_defer(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == 0) {
        check_sequence(shmem, shmem_size / sizeof(unsigned int));
    }
    else if (pid > 0) {
        sleep(1);
        write_sequence(shmem, shmem_size / sizeof(unsigned int));
    }
    else {
        perror("fork()");
        return_defer(EXIT_FAILURE);
    }

defer:
    munmap(semaphore, sizeof(sem_t));
    munmap(shmem, shmem_size);
    sem_destroy(semaphore);
    wait(NULL);
    return status;
}
