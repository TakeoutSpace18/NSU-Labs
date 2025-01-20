#ifndef CACHE_H
#define CACHE_H

#include <pthread.h>
#include <stdatomic.h>

#include "client_cond.h"
#include "hashmap.h"
#include "list.h"
#include "buffer.h"

typedef struct cache {
    hashmap_t map;
    list_t lru;

    pthread_mutex_t lock;
    size_t total_size_max;
    atomic_size_t total_size;
} cache_t;

typedef struct cache_entry {
    client_cond_t data_available;
    buffer_t data;
    buffer_t *key;
    _Atomic bool is_finished;
    cache_t *cache_p;
    pthread_rwlock_t lock;
    atomic_int ref_count;

    list_t link;
} cache_entry_t;

int cache_create(cache_t *cache, size_t max_size_bytes);
void cache_destroy(cache_t *cache);

/* methods for adding data to cache */

/* Create cache entry and add to cache.
 * In case entry already exists, returns pointer to existing entry
 * and sets err to EEXIST */
cache_entry_t *cache_add_entry(cache_t *cache, buffer_t *key, int *err);

void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len);

void cache_entry_finished(cache_entry_t *entry);

int cache_remove_entry(cache_t *cache, buffer_t *key);

/* methods for reading data from cache */

cache_entry_t *cache_get_entry(cache_t *cache, const buffer_t *key);

void cache_entry_put(cache_entry_t *entry);

void cache_entry_get_data(cache_entry_t *entry, char **start, size_t *len);

/* Wait for more data to become available.
 * Returns OK if new data is available,
 * ERROR if cache entry is full and no more data will be appended.
 * Sets data_len to size of available data */
int cache_entry_wait(cache_entry_t *entry, size_t *data_len);

#endif /* !CACHE_H */
