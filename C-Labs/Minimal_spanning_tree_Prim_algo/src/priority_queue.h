#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

#include "error_handling.h"

typedef struct PriorityQueue {
    void **heap; // array that is used as heap
    size_t size;
    size_t capacity;
    size_t elementSize;
    int (*cmp)(const void *, const void *);
} PriorityQueue_t;

PriorityQueue_t *createPQueue(size_t sizeToReserve, size_t elementSize, int (*cmp)(const void *, const void *));
void destroyPQueue(PriorityQueue_t **pq);

// Copy data pointed by ptr and add it to the priority queue
exitCode_t pqAdd(PriorityQueue_t *pq, const void *src);

exitCode_t pqPop(PriorityQueue_t *pq, void *dst);

bool pqIsEmpty(const PriorityQueue_t *pq);

#endif
