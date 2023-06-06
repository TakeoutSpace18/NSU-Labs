#include <stdlib.h>
#include <assert.h>

#include <stdio.h>

#include "stack.h"

Stack MakeStack() {
    Stack stack = malloc(sizeof(struct StackInstance));
    stack->curBlock = malloc(sizeof(struct StackBlock));
    stack->curBlock->prevBlock = NULL;
    stack->curIndex = 0;
    return stack;
}

void DestroyStack(Stack stack) {
    struct StackBlock* block = stack->curBlock;
    while (block) {
        struct StackBlock* prevBlock = block->prevBlock;
        free(block);
        block = prevBlock;
    }
    free(stack);
}

int IsEmpty(const Stack stack) {
    return stack->curIndex == 0 && stack->curBlock->prevBlock == NULL;
}

void Push(Stack stack, T value) {
    assert(stack != NULL);

    if (stack->curIndex == BLOCK_SIZE) {
        // allocate new block
        struct StackBlock* newBlock = malloc(sizeof(struct StackBlock));
        newBlock->prevBlock = stack->curBlock;
        stack->curBlock = newBlock;
        stack->curIndex = 0;
    }

    struct StackBlock* block = stack->curBlock;
    block->data[stack->curIndex] = value;
    stack->curIndex++;
}

T Pop(Stack stack) {
    assert(stack != NULL);
    assert(stack->curIndex != 0);

    struct StackBlock* block = stack->curBlock;
    stack->curIndex--;
    T value = block->data[stack->curIndex];

    if (stack->curIndex == 0 && block->prevBlock != NULL) {
        // go back to the previous block
        stack->curIndex = BLOCK_SIZE;
        stack->curBlock = block->prevBlock;
        free(block);
    }

    return value;
}

T GetTop(const Stack stack) {
    assert(stack != NULL);
    assert(stack->curIndex != 0);

    struct StackBlock* block = stack->curBlock;
    T value = block->data[stack->curIndex - 1];
    return value;
}
