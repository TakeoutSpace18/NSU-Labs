#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include <stdlib.h>

typedef struct {
    size_t chunkSize;
    void *allocationPointer;
} PoolAllocator_t;

PoolAllocator_t *createPoolAllocator(size_t chunkSize);

void *allocate(PoolAllocator_t *allocator);
void *callocate(PoolAllocator_t *allocator);
void deallocate(PoolAllocator_t *allocator, void *ptr);

#endif
