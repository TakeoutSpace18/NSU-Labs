#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#define MAX2(a, b) ((a) < (b) ? (b) : (a))
#define BST_TO_LEFT(val, nodeVal) ((val) <= (nodeVal) ? 1 : 0)
typedef int T;

void ExitWithMessage(const char* msg) {
    printf("%s\n", msg);
    exit(0);
}

typedef struct AVLNode {
    int8_t depth;
    T value;
    struct AVLNode* leftChild;
    struct AVLNode* rightChild;
} AVLNode;

AVLNode* MakeNode(T value, AVLNode* placeholder) {
    assert(placeholder != NULL);
    placeholder->value = value;
    placeholder->leftChild = NULL;
    placeholder->rightChild = NULL;
    placeholder->depth = 1;
    return placeholder;
}

int getDepth(AVLNode* node) {
    if (!node) return 0;
    return node->depth;
}

void updateDepth(AVLNode* node) {
    assert(node != NULL);
    int d1 = getDepth(node->leftChild);
    int d2 = getDepth(node->rightChild);
    node->depth = 1 + MAX2(d1, d2);
}

int getBalanceFactor(AVLNode* node) {
    assert(node != NULL);
    return getDepth(node->rightChild) - getDepth(node->leftChild);
}

AVLNode* RotateLeft(AVLNode* nodeA) {
    AVLNode* nodeB = nodeA->rightChild;
    nodeA->rightChild = nodeB->leftChild;
    nodeB->leftChild = nodeA;
    updateDepth(nodeA);
    updateDepth(nodeB);
    return nodeB;
}

AVLNode* RotateRight(AVLNode* nodeA) {
    AVLNode* nodeB = nodeA->leftChild;
    nodeA->leftChild = nodeB->rightChild;
    nodeB->rightChild = nodeA;
    updateDepth(nodeA);
    updateDepth(nodeB);
    return nodeB;
}

AVLNode* RotateLeftRight(AVLNode* nodeA) {
    nodeA->leftChild = RotateLeft(nodeA->leftChild);
    return RotateRight(nodeA);
}

AVLNode* RotateRightLeft(AVLNode* nodeA) {
    nodeA->rightChild = RotateRight(nodeA->rightChild);
    return RotateLeft(nodeA);
}

AVLNode* Insert(AVLNode* node, T value, AVLNode* placeholder) {
    if (node == NULL) return MakeNode(value, placeholder);

    if (BST_TO_LEFT(value, node->value)) {
        node->leftChild = Insert(node->leftChild, value, placeholder);
    }
    else {
        node->rightChild = Insert(node->rightChild, value, placeholder);
    }

    updateDepth(node);
    int balanceFactor = getBalanceFactor(node);

    if (balanceFactor < -1) {
        if (BST_TO_LEFT(value, node->leftChild->value)) {
            node = RotateRight(node);
        }
        else {
            node = RotateLeftRight(node);
        }
    }

    if (balanceFactor > 1) {
        if (BST_TO_LEFT(value, node->rightChild->value)) {
            node = RotateRightLeft(node);
        }
        else {
            node = RotateLeft(node);
        }
    }
    return node;
}

int main(void) {
    int nodesCount;
    if (scanf("%i", &nodesCount)) {
        AVLNode* memory = malloc(sizeof(AVLNode) * nodesCount);
        AVLNode* root = NULL;

        for (int i = 0; i < nodesCount; ++i) {
            int val;
            if (!scanf("%i", &val))
                ExitWithMessage("No input");
            root = Insert(root, val, &memory[i]);
        }

        printf("%i\n", getDepth(root));
        free(memory);
    }
    else {
        ExitWithMessage("No input");
    }
    return EXIT_SUCCESS;
}
