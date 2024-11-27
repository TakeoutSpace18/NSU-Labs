#ifndef STORAGE_H
#define STORAGE_H

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
};

/* not thread-safe */
storage_t generate_storage(size_t size);

#endif // !STORAGE_H
