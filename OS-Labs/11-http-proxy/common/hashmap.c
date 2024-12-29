#include "hashmap.h"

#include <pthread.h>

static void hashmap_bucket_init(hashmap_bucket_t *bucket) {
    pthread_mutex_init(&bucket->lock, NULL);
    dynarray_create(&bucket->entries, sizeof(hashmap_entry_t));
}

static inline void *hashmap_locate_bucket(const hashmap_t *hm, const void *key)
{
    size_t bucket_index = hm->hashfunc(key) % dynarray_size(&hm->buckets);
    return dynarray_at(&hm->buckets, bucket_index);
}

int hashmap_create(hashmap_t *hm, hashmap_hashfunc_t hashfunc,
                   hashmap_key_comparator_t key_comparator, size_t nr_buckets)
{
    memset(hm, 0, sizeof(*hm));

    dynarray_create(&hm->buckets, sizeof(hashmap_bucket_t));
    int err = dynarray_reserve(&hm->buckets, nr_buckets);
    if (err != OK) {
        dynarray_destroy(&hm->buckets);
        return err;
    }

    for (size_t i = 0; i < nr_buckets; ++i) {
        hashmap_bucket_t *bucket = dynarray_emplace_back(&hm->buckets);
        hashmap_bucket_init(bucket);
    }

    hm->size = 0;

    hm->hashfunc = hashfunc;
    hm->key_cmp = key_comparator;
    hm->value_destructor = NULL;
    hm->key_destructor = NULL;

    return OK;
}

void hashmap_destroy(hashmap_t *hm)
{
    hashmap_bucket_t *bucket;
    dynarray_foreach(bucket, &hm->buckets) {
        hashmap_entry_t *entry;
        dynarray_foreach(entry, &bucket->entries) {
            if (hm->key_destructor) {
                hm->key_destructor(entry->key);
            }
            if (hm->value_destructor) {
                hm->value_destructor(entry->value);
            }
        }

        dynarray_destroy(&bucket->entries);
        pthread_mutex_destroy(&bucket->lock);
    }
    dynarray_destroy(&hm->buckets);

    memset(hm, 0, sizeof(*hm));
}

int hashmap_insert(hashmap_t *hm, void *key, void *value)
{
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, key);
    pthread_mutex_lock(&bucket->lock);

    int err = dynarray_push_back(&bucket->entries, &(hashmap_entry_t) {
        .key = key,
        .value = value
    });

    if (err != OK) {
        pthread_mutex_unlock(&bucket->lock);
        return err;
    }

    pthread_mutex_unlock(&bucket->lock);
    return OK;
}

void *hashmap_find(const hashmap_t *hm, const void *key)
{
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, key);
    pthread_mutex_lock(&bucket->lock);

    hashmap_entry_t *entry;
    dynarray_foreach(entry, &bucket->entries) {
        if (hm->key_cmp(entry->key, key) == 0) {
            pthread_mutex_unlock(&bucket->lock);
            return entry->value;
        }
    }

    pthread_mutex_unlock(&bucket->lock);
    return NULL;
}

int hashmap_remove(hashmap_t *hm, const void *key)
{
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, key);
    pthread_mutex_lock(&bucket->lock);

    for (size_t i = 0; i < dynarray_size(&bucket->entries); ++i) {
        hashmap_entry_t *entry = dynarray_at(&bucket->entries, i);
        if (hm->key_cmp(entry->key, key) == 0) {
            dynarray_remove(&bucket->entries, i);
            pthread_mutex_unlock(&bucket->lock);
            return OK;
        }
    }

    pthread_mutex_unlock(&bucket->lock);
    return ENOENT;
}
