#define _GNU_SOURCE

#include <threads.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <error.h>
#include <unistd.h>
#include <pthread.h>

static thread_local char *thread_name = "undefined";

static void
block_signal(int sig)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    if (sigprocmask(SIG_BLOCK, &set, NULL)) {
        perror("sigprocmask()");
        pthread_exit(EXIT_FAILURE);
    }
}

static void
unblock_signal(int sig)
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, sig);

    if (sigprocmask(SIG_UNBLOCK, &set, NULL)) {
        perror("sigprocmask()");
        pthread_exit(EXIT_FAILURE);
    }
}

static void
sigint_handler(int signo)
{
    printf("%s: hello from SIGINT handler\n", thread_name);
}

static void*
thread_one(void *arg)
{
    thread_name = "thread_one";

    /* Block all signals */
    sigset_t set;
    sigfillset(&set);
    if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
        perror("pthread_sigmask()");
        pthread_exit(EXIT_FAILURE);
    }
    return NULL;
}

static void*
thread_two(void *arg)
{
    thread_name = "thread_two";

    unblock_signal(SIGINT);

    struct sigaction act = { 0 };
    act.sa_handler = sigint_handler;

    if (sigaction(SIGINT, &act, NULL) != 0) {
        perror("sigaction()");
        pthread_exit(EXIT_FAILURE);
    }

    sleep(5);

    return NULL;
}

static void*
thread_three(void *arg)
{
    thread_name = "thread_three";

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);

    int sig;
    if (sigwait(&set, &sig) != 0) {
        perror("sigwait()");
        pthread_exit(EXIT_FAILURE);
    }

    if (sig == SIGQUIT) {
        printf("%s: SIGQUIT recieved\n", thread_name);
    }
    return NULL;
}

static void
print_cur_sigmask()
{
    sigset_t current_mask;

    if (sigprocmask(SIG_SETMASK, NULL, &current_mask) == -1) {
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }

    printf("Current signal mask: ");
    for (int i = 1; i < NSIG; i++) {
        if (sigismember(&current_mask, i)) {
            printf("%s ", strsignal(i));
        }
    }
    printf("\n");
}

int
main() 
{
    thread_name = "thread_main";

    /* All signals are unblocked at the beginning */
    block_signal(SIGQUIT);
    block_signal(SIGINT);
    print_cur_sigmask();

    printf("pid: %i\n", getpid());
    getc(stdin);

    printf("creating threads...\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t threads[3];
    void* (*thread_mains[3])(void *) = {
        thread_one, thread_two, thread_three
    };

    /* Each thread inherits sigmask from its parent */
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], &attr, thread_mains[i], NULL) != 0) {
            perror("pthread_create()");
            return EXIT_FAILURE;
        }
    }

    sleep(1);
    kill(getpid(), SIGINT);
    sleep(1);
    kill(getpid(), SIGQUIT);
    sleep(1);

    for (int i = 0; i < 3; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join()");
            return EXIT_FAILURE;
        }
    }


    sleep(3);

    printf("finishing...\n");
    return EXIT_SUCCESS;
}
