#include "cache.h"

#include "c.h"
#include "buffer.h"
#include "client_cond.h"
#include "coroutine.h"
#include "hashmap.h"
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

    return entry;
}

static void cache_entry_destroy(cache_entry_t **entry_p)
{
    cache_entry_t *entry = *entry_p;

    buffer_free(&entry->data);
    client_cond_destroy(&entry->data_available);
    pthread_rwlock_destroy(&entry->lock);
    free(entry);

    *entry_p = NULL;
}

static inline size_t cache_entry_size(const cache_entry_t *entry)
{
    assert(entry);
    return buffer_used(&entry->data);
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
    hashmap_create(&cache->entries, buffer_hash, buffer_cmp, 128);
    hashmap_set_key_destructor(&cache->entries, key_destructor);
    hashmap_set_value_destructor(&cache->entries, value_destructor);

    cache->total_size = 0;
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
    coroutine_log_debug("Needed memory: %zu", size);
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

cache_entry_t *cache_add_entry(cache_t *cache, buffer_t *key)
{
    assert(cache);

    cache_entry_t *entry = cache_entry_create(cache);
    if (!entry) {
        return NULL;
    }

    entry->ref_count = 2; /* entry is referenced by hashmap and by appending side */
    int err = hashmap_insert(&cache->entries, key, entry);
    if (err != OK) {
        cache_entry_destroy(&entry);
        return NULL;
    }

    return entry;
}

int cache_remove_entry(cache_t *cache, buffer_t *key) {
    assert(cache);
    assert(key);

    cache_entry_t *entry = hashmap_find(&cache->entries, key);
    if (!entry) {
        return ERROR;
    }

    cache->total_size -= cache_entry_size(entry);

    entry->ref_count--; /* remove hashmap reference */
    int err = hashmap_remove(&cache->entries, key);
    if (err != OK) {
        return err;
    }

    return OK;
}

cache_entry_t *cache_get_entry(const cache_t *cache, const buffer_t *key)
{
    assert(cache);

    cache_entry_t *entry = hashmap_find(&cache->entries, key);
    if (!entry) {
        return NULL;
    }

    entry->ref_count++;
    return entry;
}

void cache_entry_put(cache_entry_t *entry)
{
    entry->ref_count--;
    if (entry->ref_count == 0) {
        cache_entry_destroy(&entry);
    }
}

void cache_entry_append_data(cache_entry_t *entry, void *p, size_t len)
{
    pthread_rwlock_wrlock(&entry->lock);

    /* TODO: think about what to do if entry
     * to delete is the currently locked entry */
    cache_ensure_memory(entry->cache_p, len);

    int err = buffer_ensure(&entry->data, len);
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
    client_cond_signal(&entry->data_available);

    pthread_rwlock_unlock(&entry->lock);
}

int cache_entry_wait(cache_entry_t *entry)
{
    assert(entry);
    if (entry->is_finished) {
        return ERROR;
    }

    client_cond_wait(&entry->data_available);

    /* in case entry was marked finished during wait */
    if (entry->is_finished) {
        return ERROR;
    }

    return OK;
}

const buffer_t *cache_entry_read_begin(cache_entry_t *entry)
{
    assert(entry);
    pthread_rwlock_rdlock(&entry->lock);
    return &entry->data;
}

void cache_entry_read_end(cache_entry_t *entry)
{
    pthread_rwlock_unlock(&entry->lock);
}
