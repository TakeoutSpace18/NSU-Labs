#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Queue.h"

Queue* CreateQueue() {
    Queue* queue = malloc(sizeof(Queue));
    queue->backNode = NULL;
    queue->frontNode = NULL;
    queue->size = 0;
    return queue;
}

static QNode* CreateQNode(T data) {
    QNode* node = malloc(sizeof(QNode));
    node->data = data;
    node->next = NULL;
    return node;
}

void DestroyQueue(Queue* queue) {
    assert(queue != NULL);
    MakeEmpty(queue);
    free(queue);
}

void Push(Queue* queue, T value) {
    assert(queue != NULL);
    QNode* newNode = CreateQNode(value);
    if (queue->size == 0) {
        queue->backNode = newNode;
        queue->frontNode = newNode;
    }
    else {
        queue->backNode->next = newNode;
        queue->backNode = newNode;
    }
    ++queue->size;
}

T Pop(Queue* queue){
    assert(queue != NULL);
    assert(queue->size != 0);

    QNode* head = queue->frontNode;
    T value = head->data;
    queue->frontNode = head->next;

    if (queue->frontNode == NULL) {
        queue->backNode = NULL;
    }
    free(head);
    --queue->size;
    return value;
}

T* GetFront(const Queue* queue) {
    assert(queue != NULL);
    assert(queue->size != 0);
    return &queue->frontNode->data;
}

size_t GetSize(const Queue* queue) {
    return queue->size;
}

int IsEmpty(const Queue* queue) {
    assert(queue != NULL);
    return queue->frontNode == NULL;
}

void MakeEmpty(Queue* queue) {
    assert(queue != NULL);
    queue->size = 0;
    QNode* current = queue->frontNode;
    while (current != NULL) {
        QNode* next = current->next;
        free(current);
        current = next;
    }
    queue->backNode = NULL;
    queue->frontNode = NULL;
}
