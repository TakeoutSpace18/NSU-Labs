#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "avl_tree.h"

AVLTree_t *avl_createTree(ComparatorFunc_t cmp, DataDestroyFunc_t data_destroy_func) {
    AVLTree_t *tree = malloc(sizeof(AVLTree_t));
    if (!tree) {
        return NULL;
    }
    tree->size = 0;
    tree->head = NULL;
    tree->cmp = cmp;
    tree->data_destroy_func = data_destroy_func;
    return tree;
}

static void destroyTreeNodes(AVLNode_t *node) {
    if (!node) {
        return;
    }
    if (node->leftChild) {
        destroyTreeNodes(node->leftChild);
    }
    if (node->rightChild) {
        destroyTreeNodes(node->rightChild);
    }
    free(node);
}

void avl_destroyTree(AVLTree_t **tree) {
    if (!tree || !(*tree)) {
        return;
    }
    destroyTreeNodes((*tree)->head);
    free(*tree);
    *tree = NULL;
}

static int getDepth(AVLNode_t* node) {
    if (!node) return 0;
    return node->depth;
}

static int max2(int a, int b) {
    return a > b ? a : b;
}

static void updateDepth(AVLNode_t *node) {
    int d1 = getDepth(node->leftChild);
    int d2 = getDepth(node->rightChild);
    node->depth = 1 + max2(d1, d2);
}

static int getBalanceFactor(AVLNode_t *node) {
    if (!node) {
        return 0;
    }
    return getDepth(node->rightChild) - getDepth(node->leftChild);
}

static AVLNode_t *rotateLeft(AVLNode_t *nodeA) {
    AVLNode_t* nodeB = nodeA->rightChild;
    nodeA->rightChild = nodeB->leftChild;
    nodeB->leftChild = nodeA;
    updateDepth(nodeA);
    updateDepth(nodeB);
    return nodeB;
}

static AVLNode_t *rotateRight(AVLNode_t *nodeA) {
    AVLNode_t* nodeB = nodeA->leftChild;
    nodeA->leftChild = nodeB->rightChild;
    nodeB->rightChild = nodeA;
    updateDepth(nodeA);
    updateDepth(nodeB);
    return nodeB;
}

static AVLNode_t *rotateLeftRight(AVLNode_t *nodeA) {
    nodeA->leftChild = rotateLeft(nodeA->leftChild);
    return rotateRight(nodeA);
}

static AVLNode_t* rotateRightLeft(AVLNode_t *nodeA) {
    nodeA->rightChild = rotateRight(nodeA->rightChild);
    return rotateLeft(nodeA);
}

static AVLNode_t *createNode(void *value) {
    AVLNode_t *node = malloc(sizeof(AVLNode_t));
    if (!node) {
        return NULL;
    }
    node->data = value;
    node->leftChild = NULL;
    node->rightChild = NULL;
    node->depth = 1;
    return node;
}

static AVLNode_t *rebalance(AVLNode_t *node) {
    int balanceFactor = getBalanceFactor(node);

    if (balanceFactor < -1) {
        if (getBalanceFactor(node->leftChild) < 0) {
            return rotateRight(node);
        }
        else {
            return rotateLeftRight(node);
        }
    }
    else if (balanceFactor > 1) {
        if (getBalanceFactor(node->rightChild) > 0) {
            return rotateLeft(node);
        }
        else {
            return rotateRightLeft(node);
        }
    }
    return node;
}

static AVLNode_t *insertImpl(AVLTree_t *tree, AVLNode_t *curNode, void *data, bool *inserted) {
    if (curNode == NULL) {
        *inserted = true;
        return createNode(data);
    }
    int cmpResult = tree->cmp(data, curNode->data);
    if (cmpResult < 0) {
        curNode->leftChild = insertImpl(tree, curNode->leftChild, data, inserted);
    }
    else if (cmpResult > 0) {
        curNode->rightChild = insertImpl(tree, curNode->rightChild, data, inserted);
    }
    else {
        // if node with passed data already exist
        // free previous data and put new
        *inserted = false;
        if (tree->data_destroy_func) {
            tree->data_destroy_func(curNode->data);
        }
        curNode->data = data;
        return curNode;
    }

    updateDepth(curNode);
    curNode = rebalance(curNode);
    return curNode;
}

bool avl_insert(AVLTree_t *tree, void *data) {
    bool inserted = false;
    tree->head = insertImpl(tree, tree->head, data, &inserted);
    if (inserted) {
        ++tree->size;
    }
    return inserted;
}

static AVLNode_t *findMinimalNode(AVLNode_t *node) {
    while (node->leftChild != NULL) {
        node = node->leftChild;
    }
    return node;
}

static void swap(void **a, void **b) {
    void *tmp = *a;
    *a = *b;
    *b = tmp;
}

static AVLNode_t *removeImpl(AVLTree_t *tree, AVLNode_t *curNode, void *data, void **removedData) {
    if (curNode == NULL) {
        *removedData = NULL;
        return NULL;
    }

    int cmpResult = tree->cmp(data, curNode->data);
    if (cmpResult < 0) {
        curNode->leftChild = removeImpl(tree, curNode->leftChild, data, removedData);
    }
    else if (cmpResult > 0) {
        curNode->rightChild = removeImpl(tree, curNode->rightChild, data, removedData);
    }
    else {
        // node with only 1 child or no child
        if (curNode->leftChild == 0 || curNode->rightChild == 0) {
            AVLNode_t *tmp = curNode->leftChild ? curNode->leftChild : curNode->rightChild;
            *removedData = curNode->data;
            if (!tmp) { // if no children
                tmp = curNode;
                curNode = NULL;
            }
            else {
                *curNode = *tmp;
            }
            free(tmp);
        }
        else { // node with 2 children
            AVLNode_t *successor = findMinimalNode(curNode->rightChild);
            swap(&successor->data, &curNode->data);
            curNode->rightChild = removeImpl(tree, curNode->rightChild, successor->data, removedData);
        }
    }

    if (curNode == NULL) {
        return NULL;
    }

    updateDepth(curNode);
    curNode = rebalance(curNode);
    return curNode;
}

void *avl_remove(AVLTree_t *tree, void *data) {
    void *removedData;
    tree->head = removeImpl(tree, tree->head, data, &removedData);
    if (removedData != NULL) {
        --tree->size;
    }
    return removedData;
}

void *avl_find(AVLTree_t *tree, void *data) {
    if (!tree) {
        return NULL;
    }

    AVLNode_t *curNode = tree->head;
    while (curNode) {
        int cmpResult = tree->cmp(data, curNode->data);
        if (cmpResult < 0) {
            curNode = curNode->leftChild;
        }
        else if (cmpResult > 0) {
            curNode = curNode->rightChild;
        }
        else {
            return curNode->data;
        }
    }
    return NULL;
}

size_t avl_getSize(const AVLTree_t *tree) {
    return tree->size;
}

static void foreachImpl(AVLNode_t *curNode, TraverseFunc_t func, void *context) {
    if (!curNode) {
        return;
    }
    foreachImpl(curNode->leftChild, func, context);
    func(curNode->data, context);
    foreachImpl(curNode->rightChild, func, context);
}

void avl_foreach(const AVLTree_t *tree, TraverseFunc_t func, void *context) {
    foreachImpl(tree->head, func, context);
}
