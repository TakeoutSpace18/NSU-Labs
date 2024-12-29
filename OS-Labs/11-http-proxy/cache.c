#include "cache.h"
#include "buffer.h"
#include "client_cond.h"
#include "coroutine.h"
#include "hashmap.h"
#include <pthread.h>
#include <string.h>

static hash_t buffer_hash(const void *data)
{
    const buffer_t *buf = (const buffer_t *) data;
    return murmurhash1(buf->start, buffer_used(buf));
}

static int buffer_cmp(const void *lhs, const void *rhs)
{
    if (buffer_used(lhs) != buffer_used(rhs)) {
        return -1;
    }

    return memcmp(lhs, rhs, buffer_used(lhs));
}

static void key_destructor(void *key)
{
    buffer_t *buf = (buffer_t *) key;
    buffer_free(buf);
    free(buf);
}

static void value_destructor(void *value)
{
    cache_entry_t *entry = (cache_entry_t *) value;
    cache_entry_destroy(&entry);
}

int cache_create(cache_t *cache, size_t max_size_bytes)
{
    memset(cache, 0, sizeof(cache_t));
    hashmap_create(&cache->entries, buffer_hash, buffer_cmp, 128);
    hashmap_set_key_destructor(&cache->entries, key_destructor);
    hashmap_set_value_destructor(&cache->entries, value_destructor);

    cache->total_size_max = max_size_bytes;

    return OK;
}

void cache_destroy(cache_t *cache)
{
    hashmap_destroy(&cache->entries);
    memset(cache, 0, sizeof(*cache));
}

static int cache_free_memory(cache_t *cache, size_t size)
{
    assert(size <= cache->total_size_max);

    /* not implemented yet */
    return OK;
}

static void cache_ensure_memory(cache_t *cache, size_t size) {
    if (cache->total_size + size > cache->total_size_max) {
        size_t needed_memory = cache->total_size + size - cache->total_size_max;
        int err = cache_free_memory(cache, needed_memory);
        if (err != OK) {
            coroutine_log_fatal("Failed to free memory in cache");
            abort();
        }
    }
}

int cache_add_entry(cache_t *cache, buffer_t *key, cache_entry_t *entry)
{
    assert(cache);

    cache_ensure_memory(cache, cache_entry_size(entry));

    int err = hashmap_insert(&cache->entries, key, entry);
    if (err != OK) {
        return err;
    }

    cache->total_size += cache_entry_size(entry);

    return OK;
}

cache_entry_t *cache_entry_create(cache_t *cache)
{
    assert(cache);

    cache_entry_t *entry = calloc(1, sizeof(*entry));
    if (!entry) {
        return NULL;
    }

    client_cond_create(&entry->data_available);
    buffer_init(&entry->data);
    entry->cache_p = cache;

    return entry;
}

void cache_entry_destroy(cache_entry_t **entry_p)
{
    cache_entry_t *entry = *entry_p;

    buffer_free(&entry->data);
    client_cond_destroy(&entry->data_available);
    pthread_rwlock_destroy(&entry->lock);
    free(entry);

    *entry_p = NULL;
}

void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len)
{
    /* not implemented yet */
}

void cache_entry_data_wait(cache_entry_t *entry)
{
    assert(entry);
    client_cond_wait(&entry->data_available);
}
