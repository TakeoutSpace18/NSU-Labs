#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int global = 42;
pthread_mutex_t mythread_mutex;

#define BUFSIZE 512
void *mythread(void *arg) {
    pthread_t *thread = (pthread_t *) arg;

    pthread_mutex_lock(&mythread_mutex);

    char buf[BUFSIZE];
	ssize_t len = 0;
    len += snprintf(buf + len, BUFSIZE - len,
                    "mythread [%d %d %d]: Hello from mythread!\n",
                     getpid(), getppid(), gettid());
    len += snprintf(buf + len, BUFSIZE - len,
                    "\tpthread_self: %lu\n",
                    pthread_self());
    len += snprintf(buf + len, BUFSIZE - len,
                    "\ttid from pthread_create: %lu\n",
                    *thread);
    len += snprintf(buf + len, BUFSIZE - len,
                    "\tequal: %i\n\n",
                    pthread_equal(pthread_self(), *thread));

    int local = 1;
    static int static_local = 2;
    const int const_local = 3;

    len += snprintf(buf + len, BUFSIZE - len,
                    "\tlocal = %i (%p)\n"
                    "\tstatic_local = %i (%p)\n"
                    "\tconst_local = %i (%p)\n"
                    "\tglobal = %i (%p)\n\n",
                    local, &local,
                    static_local, &static_local,
                    const_local, &const_local,
                    global, &global);

    local = pthread_self() % 100;
    global = pthread_self() % 100;

    len += snprintf(buf + len, BUFSIZE - len,
                    "\t[ changed local & global ]\n"
                    "\tlocal = %i (%p)\n"
                    "\tglobal = %i (%p)\n",
                    local, &local,
                    global, &global);

    pthread_mutex_unlock(&mythread_mutex);
    puts(buf);
	return NULL;
}

int main() {
	pthread_t tid[5];

	printf("main [%d %d %d]: Hello from main!\n", getpid(), getppid(), gettid());

    pthread_mutex_init(&mythread_mutex, NULL);

    for (int i = 0; i < 5; ++i) {
        if (i == 0)
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        int err = pthread_create(&tid[i], NULL, mythread, &tid[i]);
        if (i == 0)
            printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        if (err) {
            printf("main: pthread_create() failed: %s\n", strerror(err));
            return -1;
        }
    }

    for (int i = 0; i < 5; ++i) {
        int err = pthread_join(tid[i], NULL);
        if (err) {
            printf("main: pthread_join() failed: %s\n", strerror(err));
            return -1;
        }

    }

    getc(stdin);

	return 0;
}

