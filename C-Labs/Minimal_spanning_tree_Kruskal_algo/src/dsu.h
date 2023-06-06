#ifndef DSU_H
#define DSU_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

// Disjoint set union
typedef struct DSU {
    uint32_t *link;
    uint32_t size;
} DSU_t;

DSU_t *createDSU(uint32_t size);
void destroyDSU(DSU_t **set);

int64_t find(const DSU_t *set, uint32_t a);
void unite(DSU_t *set, uint32_t a, uint32_t b);

#endif
