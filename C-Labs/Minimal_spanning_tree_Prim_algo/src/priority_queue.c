#include "priority_queue.h"

#include <stdlib.h>
#include <string.h>

#include "error_handling.h"

const float realloc_factor = 1.5f;

PriorityQueue_t *createPQueue(size_t sizeToReserve, size_t elementSize, int (*cmp)(const void *, const void *)) {
    if (!cmp) {
        log_error("no comparator provided");
        return NULL;
    }

    PriorityQueue_t *pq = malloc(sizeof(PriorityQueue_t));
    if (!pq) {
        log_error("malloc() failed");
        return NULL;
    }

    pq->heap = malloc(sizeof(void*) * sizeToReserve);
    if (!pq->heap) {
        log_error("malloc() failed");
        free(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = sizeToReserve;
    pq->elementSize = elementSize;
    pq->cmp = cmp;
    return pq;
}

void destroyPQueue(PriorityQueue_t **pq) {
    if (!pq || !(*pq)) {
        return;
    }
    for (size_t i = 0; i < (*pq)->size; ++i) {
        free((*pq)->heap[i]);
    }
    free((*pq)->heap);
    free(*pq);
    *pq = NULL;
}

static void swap(void **a, void **b) {
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapifyBottomUp(PriorityQueue_t *pq, size_t node) {
    if (node == 0) {
        return;
    }

    size_t root = (node - 1) / 2;
    if (pq->cmp(pq->heap[root], pq->heap[node]) < 0) {
        swap(&pq->heap[root], &pq->heap[node]);
        heapifyBottomUp(pq, root);
    }
}

exitCode_t pqAdd(PriorityQueue_t *pq, const void *ptr) {
    if (!pq || !ptr) {
        log_error("NULL pointer passed");
        return INVALID_ARGUMENT;
    }

    void *data = malloc(pq->elementSize);
    if (!data) {
        log_error("malloc() failed");
        return ALLOCATION_ERROR;
    }

    memcpy(data, ptr, pq->elementSize);

    if (pq->size == pq->capacity) {
        pq->capacity *= realloc_factor;
        pq->heap = realloc(pq->heap, pq->capacity * sizeof(void*));
        if (!pq->heap) {
            log_error("realloc() failed");
            free(data);
            return ALLOCATION_ERROR;
        }
    }

    // heap add logic
    pq->heap[pq->size] = data;
    heapifyBottomUp(pq, pq->size);
    pq->size++;
    return SUCCESS;
}


static void heapifyUpBottom(PriorityQueue_t *pq, size_t root) {
    size_t left = root * 2 + 1;
    size_t right = root * 2 + 2;
    size_t largest = root;

    if (left < pq->size && pq->cmp(pq->heap[largest], pq->heap[left]) < 0) {
        largest = left;
    }
    if (right < pq->size && pq->cmp(pq->heap[largest], pq->heap[right]) < 0) {
        largest = right;
    }
    if (pq->heap[largest] != pq->heap[root]) {
        // move largest child to the root
        swap(&pq->heap[root], &pq->heap[largest]);
        // recursively heapify child that has been changed
        heapifyUpBottom(pq, largest);
    }
}

exitCode_t pqPop(PriorityQueue_t *pq, void *dst) {
    if (!pq || !dst) {
        log_error("NULL pointer passed");
        return INVALID_ARGUMENT;
    }

    if (pq->size == 0) {
        log_error("priority queue is empty");
        return OUT_OF_BOUNDS;
    }

    memcpy(dst, pq->heap[0], pq->elementSize);
    free(pq->heap[0]);

    pq->size--;
    // move last element to the begining
    pq->heap[0] = pq->heap[pq->size];
    // restore heap state
    heapifyUpBottom(pq, 0);
    return SUCCESS;
}

bool pqIsEmpty(const PriorityQueue_t *pq) {
    return pq->size == 0;
}
