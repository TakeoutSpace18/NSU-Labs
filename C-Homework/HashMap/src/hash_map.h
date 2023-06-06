#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <stdlib.h>

#include "avl_tree.h"

typedef struct HashMap {
    AVLTree_t **buckets;
    size_t size;
    size_t capacity;
    float loadFactor;
    size_t threshold;
} HashMap_t;

HashMap_t *hm_create();
void hm_destroy(HashMap_t **hashMap);

bool hm_put(HashMap_t *hashMap, const void *key, size_t keySize, const void *data);
void *hm_remove(HashMap_t *hashMap, const void *key, size_t keySize);
void *hm_get(HashMap_t *hashMap, const void *key, size_t keySize);

void hm_setLoadFactor(HashMap_t *hashMap, float loadFactor);
size_t hm_getSize(HashMap_t *hashMap);

void hm_printStatistics(FILE *stream, HashMap_t *hashMap);

#endif
