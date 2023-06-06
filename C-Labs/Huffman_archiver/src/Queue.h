#pragma once
#include <stdlib.h>

#include "HuffmanTree.h"

typedef HSortNode T; // queue data type

typedef struct QNode {
    struct QNode* next;
    T data;
} QNode;

typedef struct Queue {
    struct QNode* frontNode;
    struct QNode* backNode;
    size_t size;
} Queue;


Queue* CreateQueue();
void DestroyQueue(Queue* queue);

void Push(Queue* queue, T value);
T Pop(Queue* queue);

T* GetFront(const Queue* queue);
size_t GetSize(const Queue* queue);

int IsEmpty(const Queue* queue);
void MakeEmpty(Queue* queue);
