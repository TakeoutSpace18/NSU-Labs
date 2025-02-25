#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#include "queue.h"

void *qmonitor(void *arg) {
	queue_t *q = (queue_t *)arg;

	printf("qmonitor: [%d %d %d]\n", getpid(), getppid(), gettid());

	while (1) {
		queue_print_stats(q);
		sleep(1);
	}

	return NULL;
}

queue_t* queue_init(int max_count) {
	int err;

	queue_t *q = malloc(sizeof(queue_t));
	if (!q) {
		printf("Cannot allocate memory for a queue\n");
		abort();
	}

	q->first = NULL;
	q->last = NULL;
	q->max_count = max_count;
	q->count = 0;

	q->add_attempts = q->get_attempts = 0;
	q->add_count = q->get_count = 0;

    q->lock = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    q->non_empty_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
    q->non_full_cond = (pthread_cond_t) PTHREAD_COND_INITIALIZER;

	err = pthread_create(&q->qmonitor_tid, NULL, qmonitor, q);
	if (err) {
		printf("queue_init: pthread_create() failed: %s\n", strerror(err));
		abort();
	}

	return q;
}

void queue_destroy(queue_t *q) {
    if (pthread_cancel(q->qmonitor_tid) == -1) {
        fprintf(stderr, "Falied to cancel qmonitor thread: %s", strerror(errno));
        abort();
    }

    if (pthread_join(q->qmonitor_tid, NULL) == -1) {
        fprintf(stderr, "Falied to cancel qmonitor thread: %s", strerror(errno));
        abort();
    }

    qnode_t *n = q->first;
    while (n) {
        qnode_t *tmp = n;
        n = n->next;
        free(tmp);
    }

    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->non_full_cond);
    pthread_cond_destroy(&q->non_empty_cond);

    free(q);
}

int queue_add(queue_t *q, int val) {
	q->add_attempts++;

    pthread_mutex_lock(&q->lock);

    assert(q->count <= q->max_count);

    while (q->count == q->max_count) {
        pthread_cond_wait(&q->non_full_cond, &q->lock);
    }

	qnode_t *new = malloc(sizeof(qnode_t));
	if (!new) {
		printf("Cannot allocate memory for new node\n");
		abort();
	}

	new->val = val;
	new->next = NULL;

	if (!q->first)
		q->first = q->last = new;
	else {
		q->last->next = new;
		q->last = q->last->next;
	}

	q->count++;

    pthread_cond_signal(&q->non_empty_cond);
    pthread_mutex_unlock(&q->lock);

    q->add_count++;
	return 1;
}



int queue_get(queue_t *q, int *val) {
	q->get_attempts++;

    pthread_mutex_lock(&q->lock);

	assert(q->count >= 0);

    while (q->count == 0) {
        pthread_cond_wait(&q->non_empty_cond, &q->lock);
    }

	qnode_t *tmp = q->first;

	*val = tmp->val;
	q->first = q->first->next;

    free(tmp);
	q->count--;

    pthread_cond_signal(&q->non_full_cond);
    pthread_mutex_unlock(&q->lock);

    q->get_count++;
	return 1;
}

void queue_print_stats(queue_t *q) {
	printf("queue stats: current size %d; attempts: (%ld %ld %ld); counts (%ld %ld %ld)\n",
		q->count,
		q->add_attempts, q->get_attempts, q->add_attempts - q->get_attempts,
		q->add_count, q->get_count, q->add_count -q->get_count);
}

