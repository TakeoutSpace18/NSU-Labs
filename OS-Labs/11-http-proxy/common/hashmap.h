#ifndef HASHMAP_H
#define HASHMAP_H

#include "c.h"
#include "dynarray.h"

typedef uint32_t hash_t;

typedef hash_t (*hashmap_hashfunc_t)(const void *key);
typedef int (*hashmap_key_comparator_t)(const void *lhs, const void *rhs);

typedef struct hashmap_entry {
    hash_t hash;
    void *key;
    void *value;
} hashmap_entry_t;

typedef struct hashmap_bucket {
    dynarray_t entries;
} hashmap_bucket_t;

typedef struct hashmap {
    dynarray_t buckets;
    size_t size;
    hashmap_hashfunc_t hashfunc;
    hashmap_key_comparator_t key_cmp;
} hashmap_t;

int hashmap_create(hashmap_t *hm, hashmap_hashfunc_t hashfunc,
                   hashmap_key_comparator_t key_comparator, size_t nr_buckets);
void hashmap_destroy(hashmap_t *hm);

/* may return EEXIST and set old_value, new value is not inserted */
int hashmap_insert(hashmap_t *hm, void *key, void *value, void **old_value);

/* may return ENOENT */
int hashmap_remove(hashmap_t *hm, const void *key, hashmap_entry_t *entry_ret);

/* may returnn ENOENT */
int hashmap_find(const hashmap_t *hm, const void *key, hashmap_entry_t *entry_ret);

static inline size_t
hashmap_size(const hashmap_t *hm)
{
    return hm->size;
}

#define hashmap_foreach(key, value, hashmap)        \
hashmap_bucket_t *__bucket;                         \
hashmap_entry_t *__entry;                           \
dynarray_foreach(__bucket, &hashmap->buckets)       \
    dynarray_foreach(__entry, &__bucket->entries)

#endif /* HASHMAP_H */
