#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "dsu.h"

DSU_t *createDSU(uint32_t size) {
    DSU_t *set = malloc(sizeof(DSU_t));
    if (!set) {
        return NULL;
    }

    set->link = malloc(sizeof(uint32_t) * size);
    if (!set->link) {
        free(set);
        return NULL;
    }

    for (uint32_t i = 0; i < size; ++i) {
        set->link[i] = i;
    }

    set->size = size;
    return set;
}

void destroyDSU(DSU_t **set) {
    if (!set) {
        return;
    }
    free((*set)->link);
    free(*set);
    *set = NULL;
}

// returns -1 if error
int64_t find(const DSU_t *set, uint32_t a) {
    if (!set || a >= set->size) {
        return -1;
    }

    while (set->link[a] != a) {
        a = set->link[a];
    }
    return a;
}

static void swap(uint32_t *a, uint32_t *b) {
    uint32_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void unite(DSU_t *set, uint32_t a, uint32_t b) {
    a = find(set, a);
    b = find(set, b);

    // random heuristics
    bool doSwap = rand() % 2;
    if (doSwap) {
        swap(&a, &b);
    }

    set->link[a] = b;
}
