/* Dynamic array */
#ifndef DYNARRAY_H
#define DYNARRAY_H

#include "c.h"

#define DYNARRAY_START_CAPACITY 10

typedef struct dynarray {
    size_t elem_size;
    size_t size;
    size_t capacity;
    char *data;
} dynarray_t;

#define dynarray_foreach(obj, array) \
for ((obj) = (void *)(array)->data; \
     (char *)(obj) < (array)->data + ((array)->size * (array)->elem_size); \
     (obj) = (void *)((char *)(obj) + (array)->elem_size))

static inline size_t __dynarray_grow(size_t n) {
    if (n == 0) {
        return DYNARRAY_START_CAPACITY;
    }
    return (n * 15 + 9) / 10; /* multiply by 1.5 rounding up using integers */
}

static inline int
dynarray_reserve(dynarray_t *ar, size_t capacity)
{
    assert(ar);

    if (capacity <= ar->capacity) {
        return OK;
    }

    char *p = realloc(ar->data, capacity * ar->elem_size);
    if (!p) {
        return ENOMEM;
    }

    ar->capacity = capacity;
    ar->data = p;

    return OK;
}

static inline void
dynarray_create(dynarray_t *ar, size_t element_size)
{
    assert(ar);

    ar->size = 0;
    ar->capacity = 0;
    ar->elem_size = element_size;
    ar->data = NULL;
}

static inline void
dynarray_destroy(dynarray_t *ar)
{
    assert(ar);

    free(ar->data);
    memset(ar, 0, sizeof(*ar));
}

/* Get number of elements */
static inline size_t
dynarray_size(const dynarray_t *ar)
{
    assert(ar);
    return ar->size;
}

/* Add elements */
static inline int
dynarray_push_back(dynarray_t *ar, void *obj)
{
    assert(ar);
    assert(obj);

    if (ar->size + 1 > ar->capacity) {
        int err = dynarray_reserve(ar, __dynarray_grow(ar->capacity));
        if (err != OK) {
            return err;
        }
    }

    memcpy(&ar->data[ar->size * ar->elem_size], obj, ar->elem_size);
    ar->size++;

    return OK;
}

static inline void *
dynarray_emplace_back(dynarray_t *ar)
{
    assert(ar);

    if (ar->size + 1 > ar->capacity) {
        int err = dynarray_reserve(ar, __dynarray_grow(ar->capacity));
        if (err != OK) {
            return NULL;
        }
    }

    void *obj = &ar->data[ar->size * ar->elem_size];
    ar->size++;

    return obj;
}

/* Get pointer to element at index */
static inline void *
dynarray_at(const dynarray_t *ar, size_t idx)
{
    assert(ar);
    assert(idx < ar->size);

    return &ar->data[idx * ar->elem_size];
}

static inline int
dynarray_insert(dynarray_t *ar, size_t idx, void *obj)
{
    assert(ar);
    assert(idx <= ar->size);

    if (ar->size + 1 > ar->capacity) {
        int err = dynarray_reserve(ar, __dynarray_grow(ar->capacity));
        if (err != OK) {
            return err;
        }
    }

    void *dest = &ar->data[(idx + 1) * ar->elem_size];
    void *src = &ar->data[idx * ar->elem_size];
    size_t sz = (ar->size - idx) * ar->elem_size;
    memmove(dest, src, sz);

    memcpy(&ar->data[ar->size * ar->elem_size], obj, ar->elem_size);
    ar->size++;

    return OK;
}

/* Remove element at index */
static inline void
dynarray_remove(dynarray_t *ar, size_t idx)
{
    assert(ar);
    assert(idx < ar->size);

    void *dest = &ar->data[idx * ar->elem_size];
    void *src = &ar->data[(idx + 1) * ar->elem_size];
    size_t sz = (ar->size - idx - 1) * ar->elem_size;
    memmove(dest, src, sz);

    --ar->size;
}

/* Remove all elements */
static inline void dynarray_reset(dynarray_t *ar)
{
    ar->size = 0;
}

#endif /* !DYNARRAY_H */
