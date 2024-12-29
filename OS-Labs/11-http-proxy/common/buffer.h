#ifndef BUFFER_H
#define BUFFER_H

#include "c.h"

typedef struct buffer {
	char *start;
	char *pos;
	char *end;
} buffer_t;

static inline void buffer_init(buffer_t *buf)
{
	buf->start = NULL;
	buf->pos = NULL;
	buf->end = NULL;
}

static inline void buffer_free(buffer_t *buf)
{
	if (buf->start == NULL)
		return;
	free(buf->start);
	buf->start = NULL;
	buf->pos = NULL;
	buf->end = NULL;
}

static inline size_t buffer_size(const buffer_t *buf)
{
	return buf->end - buf->start;
}

static inline size_t buffer_used(const buffer_t *buf)
{
	return buf->pos - buf->start;
}

static inline size_t buffer_unused(const buffer_t *buf)
{
	return buf->end - buf->pos;
}

static inline void buffer_reset(buffer_t *buf)
{
	buf->pos = buf->start;
}

static inline int buffer_ensure(buffer_t *buf, int size)
{
	if (buf->end - buf->pos >= size) {
        return OK;
    }

	int sz = buffer_size(buf) * 2;
	int actual = buffer_used(buf) + size;
	if (actual > sz)
		sz = actual;
	char *p;
	p = realloc(buf->start, sz);
	if (p == NULL)
		return ENOMEM;

	buf->pos = p + (buf->pos - buf->start);
	buf->end = p + sz;
	buf->start = p;

	assert((buf->end - buf->pos) >= size);
	return OK;
}

static inline void buffer_advance(buffer_t *buf, size_t size)
{
	buf->pos += size;
}

static inline int buffer_add(buffer_t *buf, void *pointer, int size)
{
	int rc = buffer_ensure(buf, size);
	if (rc == -1)
		return ERROR;
	memcpy(buf->pos, pointer, size);
	buffer_advance(buf, size);
	return OK;
}

#endif
