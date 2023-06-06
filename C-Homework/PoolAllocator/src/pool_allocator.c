#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pool_allocator.h"

static const size_t chunksPerBlock = 5;

typedef struct {
    void *next;
} Chunk_t;

PoolAllocator_t *createPoolAllocator(size_t chunkSize) {
    // chunkSize should be big enough to fit pointer to the next chunk
    assert(chunkSize >= sizeof(void*));

    PoolAllocator_t *alloc = malloc(sizeof(PoolAllocator_t));
    if (!alloc) {
        return NULL;
    }
    alloc->allocationPointer = NULL;
    alloc->chunkSize = chunkSize;
    return alloc;
}

static void *allocateBlock(PoolAllocator_t *alloc) {
    assert(alloc);

    size_t blockSize = chunksPerBlock * alloc->chunkSize;
    void *block = malloc(blockSize);
    if (!block) {
        return NULL;
    }

    // connect chunks into chain
    Chunk_t* curChunk = block;
    for (size_t i = 0; i < chunksPerBlock - 1; ++i) {
        curChunk->next = (void*)curChunk + alloc->chunkSize;
        curChunk = curChunk->next;
    }

    // next pointer in the last chunk should be NULL
    curChunk->next = NULL;

    return block;
}

void *allocate(PoolAllocator_t *alloc) {
    assert(alloc);

    // allocate new block if nessecery
    if (!alloc->allocationPointer) {
        void *newBlock = allocateBlock(alloc);
        if (!newBlock) {
            return NULL;
        }
        alloc->allocationPointer = newBlock;
    }

    void *curChunk = alloc->allocationPointer;
    void *nextChunk = ((Chunk_t*)curChunk)->next;
    alloc->allocationPointer = nextChunk;

    return curChunk;
}

void *callocate(PoolAllocator_t *alloc) {
    assert(alloc);
    void *ptr = allocate(alloc);
    memset(ptr, 0, alloc->chunkSize);
    return ptr;
}

void deallocate(PoolAllocator_t *alloc, void *ptr) {
    if (!ptr) {
        return;
    }
    ((Chunk_t*)ptr)->next = alloc->allocationPointer;
    alloc->allocationPointer = ptr;
}
