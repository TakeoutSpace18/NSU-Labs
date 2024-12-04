#ifndef __FITOS_QUEUE_H__
#define __FITOS_QUEUE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

// #define USE_SPINLOCK
// #define USE_MUTEX
// #define USE_CONDVAR
// #define USE_SEMAPHORE

typedef struct _QueueNode {
	int val;
	struct _QueueNode *next;
} qnode_t;

typedef struct _Queue {
	qnode_t *first;
	qnode_t *last;

	pthread_t qmonitor_tid;

	int count;
	int max_count;

	// queue statistics
	long add_attempts;
	long get_attempts;
	long add_count;
	long get_count;

#if defined USE_SPINLOCK
    pthread_spinlock_t lock;
#elif defined USE_MUTEX
    pthread_mutex_t lock;
#endif

#ifdef USE_SEMAPHORE
    sem_t full_sem;
    sem_t empty_sem;
#endif

#if defined USE_CONDVAR
    pthread_cond_t non_empty_cond;
    pthread_cond_t non_full_cond;
#endif

} queue_t;

queue_t* queue_init(int max_count);
void queue_destroy(queue_t *q);
int queue_add(queue_t *q, int val);
int queue_get(queue_t *q, int *val);
void queue_print_stats(queue_t *q);

// ** add
#endif		// __FITOS_QUEUE_H__
