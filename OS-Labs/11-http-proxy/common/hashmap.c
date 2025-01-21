#include "hashmap.h"
#include "dynarray.h"

static void hashmap_bucket_init(hashmap_bucket_t *bucket) {
    dynarray_create(&bucket->entries, sizeof(hashmap_entry_t));
}

static inline hashmap_bucket_t *
hashmap_locate_bucket(const hashmap_t *hm, const hash_t hash)
{
    size_t bucket_index = hash % dynarray_size(&hm->buckets);
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

    return OK;
}

void hashmap_destroy(hashmap_t *hm)
{
    hashmap_bucket_t *bucket;
    dynarray_foreach(bucket, &hm->buckets) {
        dynarray_destroy(&bucket->entries);
    }

    dynarray_destroy(&hm->buckets);
    memset(hm, 0, sizeof(*hm));
}

int hashmap_insert(hashmap_t *hm, void *key, void *value, void **old_value)
{
    hash_t hash = hm->hashfunc(key);
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, hash);

    /* check if element already inserted */
    hashmap_entry_t *entry;
    dynarray_foreach(entry, &bucket->entries) {
        if (entry->hash != hash) {
            continue;
        }

        if (hm->key_cmp(entry->key, key) == 0) {
            if (old_value) {
                *old_value = entry->value;
            }
            return EEXIST;
        }
    }

    int err = dynarray_push_back(&bucket->entries, &(hashmap_entry_t) {
        .hash = hash,
        .key = key,
        .value = value
    });

    if (err != OK) {
        return err;
    }

    return OK;
}

int hashmap_find(const hashmap_t *hm, const void *key, hashmap_entry_t *entry_ret)
{
    hash_t hash = hm->hashfunc(key);
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, hash);

    hashmap_entry_t *entry;
    dynarray_foreach(entry, &bucket->entries) {
        if (entry->hash != hash) {
            continue;
        }

        if (hm->key_cmp(entry->key, key) == 0) {
            *entry_ret = *entry;
            return OK;
        }
    }

    return ENOENT;
}

int hashmap_remove(hashmap_t *hm, const void *key, hashmap_entry_t *entry_ret)
{
    hash_t hash = hm->hashfunc(key);
    hashmap_bucket_t *bucket = hashmap_locate_bucket(hm, hash);

    for (size_t i = 0; i < dynarray_size(&bucket->entries); ++i) {
        hashmap_entry_t *entry = dynarray_at(&bucket->entries, i);
        if (entry->hash != hash) {
            continue;
        }

        if (hm->key_cmp(entry->key, key) == 0) {
            if (entry_ret) {
                *entry_ret = *entry;
            }
            dynarray_remove(&bucket->entries, i);

            return OK;
        }
    }

    return ENOENT;
}
