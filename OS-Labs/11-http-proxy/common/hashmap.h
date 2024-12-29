#ifndef HASHMAP_H
#define HASHMAP_H

#include "c.h"
#include "dynarray.h"

typedef uint32_t hash_t;

typedef void (*hashmap_destructor_t)(void *data);
typedef hash_t (*hashmap_hashfunc_t)(const void *key);
typedef int (*hashmap_key_comparator_t)(const void *lhs, const void *rhs);

typedef struct hashmap_entry {
    void *key;
    void *value;
} hashmap_entry_t;

typedef struct hashmap_bucket {
    dynarray_t entries;
    pthread_mutex_t lock;
} hashmap_bucket_t;

typedef struct hashmap {
    dynarray_t buckets;
    size_t size;
    hashmap_hashfunc_t hashfunc;
    hashmap_key_comparator_t key_cmp;
    hashmap_destructor_t key_destructor;
    hashmap_destructor_t value_destructor;
} hashmap_t;

int hashmap_create(hashmap_t *hm, hashmap_hashfunc_t hashfunc,
                   hashmap_key_comparator_t key_comparator, size_t nr_buckets);
void hashmap_destroy(hashmap_t *hm);

/* thread-safe */
int hashmap_insert(hashmap_t *hm, void *key, void *value);

/* thread-safe */
int hashmap_remove(hashmap_t *hm, const void *key);

/* thread-safe */
void *hashmap_find(const hashmap_t *hm, const void *key);

static inline void
hashmap_set_key_destructor(hashmap_t *hm, hashmap_destructor_t d)
{
    hm->key_destructor = d;
}

static inline void
hashmap_set_value_destructor(hashmap_t *hm, hashmap_destructor_t d)
{
    hm->value_destructor = d;
}

static inline size_t
hashmap_size(const hashmap_t *hm)
{
    return hm->size;
}

#endif /* HASHMAP_H */
