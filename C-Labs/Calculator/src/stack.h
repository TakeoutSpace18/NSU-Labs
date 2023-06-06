#include <stdlib.h>

#define BLOCK_SIZE 32
typedef int T; // stack data type

struct StackBlock {
    struct StackBlock* prevBlock;
    T data[BLOCK_SIZE];
};

struct StackInstance {
    struct StackBlock* curBlock;
    size_t curIndex;
};

typedef struct StackInstance* Stack;

Stack MakeStack();
void DestroyStack(Stack stack);
int IsEmpty(const Stack stack);
void Push(Stack stack, T value);
T Pop(Stack stack);
T GetTop(const Stack stack);
