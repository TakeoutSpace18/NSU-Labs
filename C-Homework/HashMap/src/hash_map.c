#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

#include "hash_map.h"

const float defaultLoadFactor = 0.75f;
const float capacityExpandFactor = 2.0f;
const size_t startCapacity = 10;

typedef struct HashMapEntry {
    size_t keySize;
    const void *key;
    const void *data;
    uint64_t hash;
} HashMapEntry_t;

static int entryCompare(const void *a, const void *b) {
    const HashMapEntry_t *entry_a = a;
    const HashMapEntry_t *entry_b = b;
    if (entry_a->keySize < entry_b->keySize) {
        return -1;
    }
    else if (entry_a->keySize > entry_b->keySize) {
        return 1;
    }
    else {
        if (entry_a->hash == entry_b->hash) {
            return 0;
        }
        else {
            return memcmp(entry_a->key, entry_b->key, entry_a->keySize);
        }
    }
}

static uint64_t djb2_hash(const void *data, size_t sz) {
    const uint8_t *bytes = data;
    uint64_t hash = 5381;
    for (size_t i = 0; i < sz; ++i) {
        hash = ((hash << 5) + hash) + bytes[i]; // hash * 33 + bytes[i]
    }
    return hash;
}

HashMap_t *hm_create() {
    HashMap_t *hm = malloc(sizeof(HashMap_t));
    if (!hm) {
        return NULL;
    }

    hm->size = 0;
    hm->capacity = startCapacity;
    hm_setLoadFactor(hm, defaultLoadFactor);

    hm->buckets = malloc(sizeof(AVLTree_t*) * hm->capacity);
    if (!hm->buckets) {
        free(hm);
        return NULL;
    }

    for (size_t i = 0; i < hm->capacity; ++i) {
        hm->buckets[i] = avl_createTree(&entryCompare, &free);
        if (!hm->buckets[i]) {
            for (size_t j = 0; j < i; ++j) {
                avl_destroyTree(&hm->buckets[j]);
            }
            free(hm->buckets);
            return NULL;
        }
    }
    return hm;
}

static void entryDestroyVisitor(void *node_data, void *context) {
    HashMapEntry_t *entry = node_data;
    free(entry);
}

void hm_destroy(HashMap_t **hm) {
    if (!hm || !(*hm)) {
        return;
    }

    for (size_t i = 0; i < (*hm)->capacity; ++i) {
        avl_foreach((*hm)->buckets[i], &entryDestroyVisitor, NULL);
        avl_destroyTree(&(*hm)->buckets[i]);
    }
    free((*hm)->buckets);
    free(*hm);
    *hm = NULL;
}

static void calculateThreshold(HashMap_t *hm) {
    hm->threshold = hm->loadFactor * hm->capacity;
}

static void entryTransferVisitor(void *node_data, void *context) {
    HashMapEntry_t *entry = node_data;
    HashMap_t *hm = context;

    size_t bucketIndex = entry->hash % hm->capacity;
    avl_insert(hm->buckets[bucketIndex], entry);
}

static void resize(HashMap_t *hm) {
    size_t oldCapacity = hm->capacity;
    AVLTree_t **oldBuckets = hm->buckets;

    hm->capacity *= capacityExpandFactor;
    calculateThreshold(hm);
    hm->buckets = malloc(sizeof(AVLNode_t*) * hm->capacity);

    if (!hm->buckets) {
        hm->buckets = oldBuckets;
        hm->capacity = oldCapacity;
        return;
    }

    for (size_t i = 0; i < hm->capacity; ++i) {
        hm->buckets[i] = avl_createTree(&entryCompare, &free);
        if (!hm->buckets[i]) {
            for (size_t j = 0; j < i; ++j) {
                avl_destroyTree(&hm->buckets[j]);
            }
            free(hm->buckets);
            hm->buckets = oldBuckets;
            hm->capacity = oldCapacity;
            return;
        }
    }

    for (size_t i = 0; i < oldCapacity; ++i) {
        avl_foreach(oldBuckets[i], entryTransferVisitor, hm);
        avl_destroyTree(&oldBuckets[i]);
    }

    free(oldBuckets);
}

bool hm_put(HashMap_t *hm, const void *key, size_t keySize, const void *data) {
    if (!hm || !key || !data) {
        return false;
    }

    uint64_t hash = djb2_hash(key, keySize);
    size_t bucketIndex = hash % hm->capacity;

    HashMapEntry_t *entry = malloc(sizeof(HashMapEntry_t));
    if (!entry) {
        return false;
    }

    entry->keySize = keySize;
    entry->key = key;
    entry->data = data;
    entry->hash = hash;

    bool inserted = avl_insert(hm->buckets[bucketIndex], entry);
    if (inserted) {
        ++hm->size;
        if (hm->size >= hm->threshold) {
            resize(hm);
        }
    }
    return inserted;
}

void *hm_get(HashMap_t *hm, const void *key, size_t keySize) {
    if (!hm) {
        return NULL;
    }

    uint64_t hash = djb2_hash(key, keySize);
    size_t bucketIndex = hash % hm->capacity;

    HashMapEntry_t entryToFind = {
        .hash = hash,
        .keySize = keySize,
        .key = key,
        .data = NULL
    };

    HashMapEntry_t *found = avl_find(hm->buckets[bucketIndex], &entryToFind);
    if (!found) {
        return NULL;
    }
    return (void*)found->data;
}

void *hm_remove(HashMap_t *hm, const void *key, size_t keySize) {
    if (!hm) {
        return NULL;
    }

    uint64_t hash = djb2_hash(key, keySize);
    size_t bucketIndex = hash % hm->capacity;

    HashMapEntry_t entryToFind = {
        .hash = hash,
        .keySize = keySize,
        .key = key,
        .data = NULL
    };

    const void *data = NULL;
    HashMapEntry_t *removedEntry = avl_remove(hm->buckets[bucketIndex], &entryToFind);
    if (removedEntry != NULL) {
        --hm->size;
        data = removedEntry->data;
        free(removedEntry);
    }
    return (void*)data;
}

void hm_setLoadFactor(HashMap_t *hm, float loadFactor) {
    hm->loadFactor = loadFactor;
    calculateThreshold(hm);
}

size_t hm_getSize(HashMap_t *hm) {
    return hm->size;
}

void hm_printStatistics(FILE *stream, HashMap_t *hm) {
    fprintf(stream, "\n ------------ ########### ------------ \n");
    fprintf(stream, "size: %zu\ncapacity: %zu \n", hm->size, hm->capacity);
    fprintf(stream, "buckets size:");
    for (size_t i = 0; i < hm->capacity; ++i) {
        if (i % 10 == 0) {
            fprintf(stream, "\n");
        }
        fprintf(stream, "[%zu] ", avl_getSize(hm->buckets[i]));
    }
    fprintf(stream, "\n ------------------------------------- \n");
}
