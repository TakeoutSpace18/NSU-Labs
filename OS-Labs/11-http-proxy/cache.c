#include "cache.h"

#include <pthread.h>

#include "c.h"
#include "buffer.h"
#include "client_cond.h"
#include "coroutine.h"
#include "hashmap.h"
#include "list.h"
#include "murmurhash.h"

static cache_entry_t *cache_entry_create(cache_t *cache)
{
    assert(cache);

    cache_entry_t *entry = calloc(1, sizeof(*entry));
    if (!entry) {
        return NULL;
    }

    client_cond_create(&entry->data_available);
    buffer_init(&entry->data);
    entry->is_finished = false;
    entry->cache_p = cache;
    entry->ref_count = 0;
    entry->key = NULL;

    return entry;
}

static void cache_entry_destroy(cache_entry_t **entry_p)
{
    cache_entry_t *entry = *entry_p;

    buffer_free(&entry->data);

    buffer_free(entry->key);
    free(entry->key);

    client_cond_destroy(&entry->data_available);
    pthread_rwlock_destroy(&entry->lock);
    free(entry);

    *entry_p = NULL;
}

static inline size_t cache_entry_size(const cache_entry_t *entry)
{
    assert(entry);
    return buffer_used(&entry->data) + buffer_used(entry->key);
}

static hash_t buffer_hash(const void *data)
{
    const buffer_t *buf = (const buffer_t *) data;
    return murmurhash1(buf->start, buffer_used(buf));
}

static int buffer_cmp(const void *lhs, const void *rhs)
{
    const buffer_t *lhs_buf = (const buffer_t *) lhs;
    const buffer_t *rhs_buf = (const buffer_t *) rhs;

    if (buffer_used(lhs_buf) != buffer_used(rhs_buf)) {
        return -1;
    }

    return memcmp(lhs_buf->start, rhs_buf->start, buffer_used(lhs));
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
    if (entry->ref_count == 0) {
        cache_entry_destroy(&entry);
    }
}

int cache_create(cache_t *cache, size_t max_size_bytes)
{
    memset(cache, 0, sizeof(cache_t));

    pthread_mutex_init(&cache->lock, NULL);

    hashmap_create(&cache->map, buffer_hash, buffer_cmp, 128);
    list_init(&cache->lru);

    cache->total_size = 0;
    cache->total_size_max = max_size_bytes;

    return OK;
}

void cache_destroy(cache_t *cache)
{
    pthread_mutex_lock(&cache->lock);

    list_t *it, *safe;
    list_foreach_safe(&cache->lru, it, safe) {
        cache_entry_t *entry = container_of(it, cache_entry_t, link);
        cache_entry_destroy(&entry);
    }

    pthread_mutex_unlock(&cache->lock);

    hashmap_destroy(&cache->map);
    pthread_mutex_destroy(&cache->lock);
    memset(cache, 0, sizeof(*cache));
}

static int cache_free_memory(cache_t *cache, size_t size)
{
    coroutine_log_debug("Needed memory: %zu", size);

    size_t freed = 0;
    while (freed < size) {
        if (list_empty(&cache->lru)) {
            return ERROR;
        }

        cache_entry_t *entry = container_of(list_back(&cache->lru),
                                            cache_entry_t, link);
        size_t entry_size = cache_entry_size(entry);
        cache_remove_entry(cache, entry->key);

        coroutine_log_info("Removed cache entry, freed %f MiB", entry_size / 1024.0 / 1024.0);
        freed += entry_size;
    }

    return OK;
}

static int cache_ensure_memory(cache_t *cache, size_t size) {
    pthread_mutex_lock(&cache->lock);

    if (cache->total_size + size > cache->total_size_max) {
        size_t needed_memory = cache->total_size + size - cache->total_size_max;
        int err = cache_free_memory(cache, needed_memory);
        pthread_mutex_unlock(&cache->lock);
        return err;
    }

    pthread_mutex_unlock(&cache->lock);
    return OK;
}

cache_entry_t *cache_add_entry(cache_t *cache, buffer_t *key, int *err)
{
    assert(cache);

    cache_entry_t *entry = cache_entry_create(cache);
    if (!entry) {
        *err = ENOMEM;
        return NULL;
    }

    entry->ref_count = 2; /* entry is referenced by hashmap and by appending side */

    pthread_mutex_lock(&cache->lock);

    cache_entry_t *old_entry;
    *err = hashmap_insert(&cache->map, key, entry, (void **)&old_entry);
    if (*err == EEXIST) {
        old_entry->ref_count++;
        pthread_mutex_unlock(&cache->lock);
        cache_entry_destroy(&entry);
        return old_entry;
    }
    else if (*err != OK) {
        pthread_mutex_unlock(&cache->lock);
        cache_entry_destroy(&entry);
        return NULL;
    }

    list_push_front(&cache->lru, &entry->link);
    entry->key = key;

    pthread_mutex_unlock(&cache->lock);
    return entry;
}

int cache_remove_entry(cache_t *cache, buffer_t *key) {
    assert(cache);
    assert(key);

    pthread_mutex_lock(&cache->lock);

    hashmap_entry_t hm_entry;
    int err = hashmap_remove(&cache->map, key, &hm_entry);
    if (err != OK) {
        pthread_mutex_unlock(&cache->lock);
        return err;
    }

    cache_entry_t *cache_entry = hm_entry.value;

    /* remove from LRU list */
    list_unlink(&cache_entry->link);

    cache->total_size -= cache_entry_size(cache_entry);

    pthread_rwlock_wrlock(&cache_entry->lock);
    cache_entry->ref_count--; /* remove hashmap reference */
    if (cache_entry->ref_count == 0) {
        pthread_rwlock_unlock(&cache_entry->lock);
        pthread_mutex_unlock(&cache->lock);
        cache_entry_destroy(&cache_entry);
        return OK;
    }

    pthread_rwlock_unlock(&cache_entry->lock);
    pthread_mutex_unlock(&cache->lock);
    return OK;
}

cache_entry_t *cache_get_entry(cache_t *cache, const buffer_t *key)
{
    assert(cache);

    pthread_mutex_lock(&cache->lock);

    hashmap_entry_t hm_entry;
    int err = hashmap_find(&cache->map, key, &hm_entry);
    if (err != OK) {
        pthread_mutex_unlock(&cache->lock);
        return NULL;
    }

    cache_entry_t *cache_entry = hm_entry.value;

    pthread_rwlock_wrlock(&cache_entry->lock);

    cache_entry->ref_count++;

    /* move to the front of LRU list */
    list_unlink(&cache_entry->link);
    list_push_front(&cache->lru, &cache_entry->link);

    pthread_rwlock_unlock(&cache_entry->lock);
    pthread_mutex_unlock(&cache->lock);
    return cache_entry;
}

void cache_entry_put(cache_entry_t *entry)
{
    pthread_rwlock_wrlock(&entry->lock);
    entry->ref_count--;

    if (entry->ref_count == 0) {
        pthread_rwlock_unlock(&entry->lock);
        cache_entry_destroy(&entry);
        return;
    }

    pthread_rwlock_unlock(&entry->lock);
}

void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len)
{
    int err;

    err = cache_ensure_memory(entry->cache_p, len);
    if (err != OK) {
        coroutine_log_warn("Failed to free requested amount of memory in cache");
    }

    pthread_rwlock_wrlock(&entry->lock);

    err = buffer_ensure(&entry->data, len);
    if (err == ENOMEM) {
        coroutine_log_error("out of memory");
        abort();
    }

    memcpy(entry->data.pos, p, len);
    buffer_advance(&entry->data, len);

    entry->cache_p->total_size += len;

    pthread_rwlock_unlock(&entry->lock);

    client_cond_signal(&entry->data_available);
}

void cache_entry_finished(cache_entry_t *entry)
{
    assert(entry);
    pthread_rwlock_wrlock(&entry->lock);

    entry->is_finished = true;
    entry->ref_count--;
    if (entry->ref_count == 0) {
        pthread_rwlock_unlock(&entry->lock);
        cache_entry_destroy(&entry);
        return;
    }

    pthread_rwlock_unlock(&entry->lock);

    client_cond_signal(&entry->data_available);
}

int cache_entry_wait(cache_entry_t *entry, size_t *data_len)
{
    assert(entry);
    pthread_rwlock_rdlock(&entry->lock);
    if (entry->is_finished) {
        *data_len = buffer_used(&entry->data);
        pthread_rwlock_unlock(&entry->lock);
        return ERROR;
    }
    pthread_rwlock_unlock(&entry->lock);

    client_cond_wait(&entry->data_available);

    pthread_rwlock_rdlock(&entry->lock);
    *data_len = buffer_used(&entry->data);
    pthread_rwlock_unlock(&entry->lock);
    return OK;
}

void cache_entry_read_begin(cache_entry_t *entry)
{
    pthread_rwlock_rdlock(&entry->lock);
}

void cache_entry_read_end(cache_entry_t *entry)
{
    pthread_rwlock_unlock(&entry->lock);
}

void cache_entry_get_data(cache_entry_t *entry, char **start, size_t *len)
{
    assert(entry);
    if (start) {
        *start = entry->data.start;
    }

    if (len) {
        *len = buffer_used(&entry->data);
    }
}
