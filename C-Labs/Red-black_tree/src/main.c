#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "stack.h"

#define MAX2(a, b) ((a) < (b) ? (b) : (a))
#define BST_TO_LEFT(val, nodeVal) ((val) < (nodeVal) ? 1 : 0)

void ExitWithMessage(const char* msg) {
    printf("%s\n", msg);
    exit(0);
}

typedef int NodeData;

typedef enum Color {
    RED,
    BLACK
} Color;

typedef struct RBNode {
    struct RBNode* leftChild;
    struct RBNode* rightChild;
    Color color;
    NodeData value;
} RBNode;

RBNode* MakeNode(NodeData value, RBNode* ph) {
    assert(ph != NULL);
    ph->value = value;
    ph->leftChild = NULL;
    ph->rightChild = NULL;
    ph->color = RED; // default insertion color
    return ph;
}

int IsLeftChild(RBNode* node, RBNode* parent) {
    assert(node != NULL);
    return parent && node == parent->leftChild;
}

int IsRightChild(RBNode* node, RBNode* parent) {
    assert(node != NULL);
    return parent && node == parent->rightChild;
}

int IsLined(RBNode* node, RBNode* parent, RBNode* grandparent) {
    assert(node != NULL);
    assert(parent != NULL);
    assert(grandparent != NULL);
    return (IsLeftChild(node, parent) && IsLeftChild(parent, grandparent)) ||
            (IsRightChild(node, parent) && IsRightChild(parent, grandparent));
}

void RotateLeft(RBNode* nodeA, RBNode* parent) {
    RBNode* nodeB = nodeA->rightChild;
    if (IsLeftChild(nodeA, parent)) {
        parent->leftChild = nodeB;
    }
    if (IsRightChild(nodeA, parent)) {
        parent->rightChild = nodeB;
    }
    nodeA->rightChild = nodeB->leftChild;
    nodeB->leftChild = nodeA;
}

void RotateRight(RBNode* nodeA, RBNode* parent) {
    RBNode* nodeB = nodeA->leftChild;
    if (IsLeftChild(nodeA, parent)) {
        parent->leftChild = nodeB;
    }
    if (IsRightChild(nodeA, parent)) {
        parent->rightChild = nodeB;
    }
    nodeA->leftChild = nodeB->rightChild;
    nodeB->rightChild = nodeA;
}

void swap(RBNode** a, RBNode** b) {
    RBNode* tmp = *a;
    *a = *b;
    *b = tmp;
}

void MakeLined(RBNode** node, RBNode** parent, RBNode** grandparent) {
    assert(*node != NULL);
    assert(*parent != NULL);
    if (IsLeftChild(*node, *parent)) {
        RotateRight(*parent, *grandparent);
    }
    else {
        RotateLeft(*parent, *grandparent);
    }
    swap(node, parent);
}

RBNode* getUncle(RBNode* parent, RBNode* grandparent) {
    if (parent && grandparent) {
        if (IsLeftChild(parent, grandparent)) {
            return grandparent->rightChild;
        }
        else {
            return grandparent->leftChild;
        }
    }
    return NULL;
}

RBNode* RBTreeBalance(RBNode* node, Stack ancestors) {
    RBNode* parent = IsEmpty(ancestors) ? NULL : Pop(ancestors);
    RBNode* grandparent = IsEmpty(ancestors) ? NULL : Pop(ancestors);
    RBNode* uncle = getUncle(parent, grandparent);

    // case 0: root node
    if (parent == NULL) {
        node->color = BLACK;
        return node;
    }
    // rebalance only red nodes
    if (node->color == BLACK) {
        return NULL;
    }
    // case 1: Black parent
    if (parent->color == BLACK) {
        return NULL;
    }
    // case 2: Red parent
    if (uncle && uncle->color == RED) {
        // case 2.1: Red uncle
        uncle->color = BLACK;
        parent->color = BLACK;
        grandparent->color = RED;
        return RBTreeBalance(grandparent, ancestors);
    }
    // case 2.2: Black uncle
    if (!IsLined(node, parent, grandparent)) {
        // case 2.2.1 Triangle shape -> make lined and go to case 2.1.2
        MakeLined(&node, &parent, &grandparent);
    }
    // case 2.2.2: Lined shape
    parent->color = BLACK;
    grandparent->color = RED;
    RBNode* grandGrandParent = IsEmpty(ancestors) ? NULL : GetTop(ancestors);
    if (IsLeftChild(node, parent)) {
        RotateRight(grandparent, grandGrandParent);
    }
    else {
        RotateLeft(grandparent, grandGrandParent);
    }
    return RBTreeBalance(parent, ancestors);
}

RBNode* Insert(RBNode* root, NodeData value, RBNode* placeholder) {
    Stack ancestors = MakeStack();
    RBNode* curNode = root;

    while (curNode != NULL) {
        Push(ancestors, curNode);
        if (BST_TO_LEFT(value, curNode->value)) {
            curNode = curNode->leftChild;
        }
        else {
            curNode = curNode->rightChild;
        }
    }

    curNode = MakeNode(value, placeholder);
    RBNode* parent = IsEmpty(ancestors) ? NULL : GetTop(ancestors);

    if (parent == NULL) {
        root = curNode;
    }
    else if (BST_TO_LEFT(value, parent->value)) {
        parent->leftChild = curNode;
    }
    else {
        parent->rightChild = curNode;
    }

    RBNode* newRoot = RBTreeBalance(curNode, ancestors);
    DestroyStack(ancestors);
    if (newRoot) return newRoot;
    return root;
}

int CalculateBlackHeight(RBNode* node) {
    if (!node) return 1; // all leaves (null ptr's) are black
    int h1 = CalculateBlackHeight(node->leftChild);
    int h2 = CalculateBlackHeight(node->rightChild);
    return MAX2(h1, h2) + (node->color == BLACK ? 1 : 0);
}

int main(void) {
    int nodesCount;
    if (scanf("%i", &nodesCount)) {
        if (nodesCount == 0) {
            ExitWithMessage("0");
        }

        RBNode* memory = malloc(sizeof(RBNode) * nodesCount);
        RBNode* root = NULL;

        for (int i = 0; i < nodesCount; ++i) {
            int val;
            if (scanf("%i", &val)) {
                root = Insert(root, val, &memory[i]);
            }
            else {
                ExitWithMessage("No input");
            }
        }
        printf("%i\n", CalculateBlackHeight(root));
        free(memory);
    }
    else {
        ExitWithMessage("No input");
    }
    return EXIT_SUCCESS;
}
