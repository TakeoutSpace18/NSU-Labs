#ifndef CACHE_H
#define CACHE_H

#include <pthread.h>
#include <stdatomic.h>

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
    _Atomic bool is_finished;
    cache_t *cache_p;
    pthread_rwlock_t lock;
    atomic_int ref_count;
} cache_entry_t;

int cache_create(cache_t *cache, size_t max_size_bytes);
void cache_destroy(cache_t *cache);

/* methods for adding data to cache */

cache_entry_t *cache_add_entry(cache_t *cache, buffer_t *key);
void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len);
void cache_entry_finished(cache_entry_t *entry);

int cache_remove_entry(cache_t *cache, buffer_t *key);

/* methods for reading data from cache */

cache_entry_t *cache_get_entry(const cache_t *cache, const buffer_t *key);
void cache_entry_put(cache_entry_t *entry);
const buffer_t *cache_entry_read_begin(cache_entry_t *entry);
void cache_entry_read_end(cache_entry_t *entry);

/* Wait for more data to become available.
 * Returns OK if new data is available,
 * ERROR if cache entry is full and no more data will be appended. */
int cache_entry_wait(cache_entry_t *entry);

#endif /* !CACHE_H */
