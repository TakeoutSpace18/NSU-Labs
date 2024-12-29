#ifndef CACHE_H
#define CACHE_H

#include <pthread.h>

#include "client_cond.h"
#include "hashmap.h"
#include "buffer.h"

typedef struct cache {
    hashmap_t entries;
    size_t total_size_max;
    size_t total_size;
} cache_t;

typedef struct cache_entry {
    client_cond_t data_available;
    buffer_t data;
    cache_t *cache_p;
    pthread_rwlock_t lock;
} cache_entry_t;

int cache_create(cache_t *cache, size_t max_size_bytes);
void cache_destroy(cache_t *cache);

int cache_add_entry(cache_t *cache, buffer_t *key, cache_entry_t *entry);

static inline cache_entry_t *
cache_get_entry(const cache_t *cache, const buffer_t *key)
{
    assert(cache);
    return hashmap_find(&cache->entries, key);
}

/* TODO: lock cache entry for reading, 
 * add state: FINISHED or IN_PROGRESS,
 * invalidate cache entry on broken pipe*/

cache_entry_t *cache_entry_create(cache_t *cache);
void cache_entry_destroy(cache_entry_t **entry);

void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len);

/* wait for more data to become available */
void cache_entry_data_wait(cache_entry_t *entry);

static inline size_t cache_entry_size(const cache_entry_t *entry)
{
    assert(entry);
    return buffer_used(&entry->data);
}

#endif /* !CACHE_H */
