#ifndef STORAGE_H
#define STORAGE_H

#include "lock.h"
#include <stdlib.h>

#define MAX_STRING_SIZE 100

typedef struct node node_t;
typedef struct storage storage_t;

struct node {
    char value[MAX_STRING_SIZE];
    node_t *next;
    void *lock;
};

struct storage {
    node_t *first;
    void *lock;
};

/* not thread-safe */
storage_t generate_storage(size_t size);

static inline void rlock_node(node_t *node) {
    read_lock(node->lock);
}

static inline void wlock_node(node_t *node) {
    write_lock(node->lock);
}

static inline void unlock_node(node_t *node) {
    if (node != NULL) {
        unlock(node->lock);
    }
}

#endif // !STORAGE_H
