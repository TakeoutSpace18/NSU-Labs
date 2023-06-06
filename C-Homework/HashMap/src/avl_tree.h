#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct AVLNode {
    void *data;
    struct AVLNode* leftChild;
    struct AVLNode* rightChild;
    int8_t depth;
} AVLNode_t;

typedef void (*DataDestroyFunc_t)(void *data);
typedef int (*ComparatorFunc_t)(const void* a, const void* b);

typedef struct AVLTree {
    AVLNode_t *head;
    size_t size;
    ComparatorFunc_t cmp;
    DataDestroyFunc_t data_destroy_func;
} AVLTree_t;

AVLTree_t *avl_createTree(ComparatorFunc_t cmp, DataDestroyFunc_t data_destroy_func);
void avl_destroyTree(AVLTree_t **tree);

bool avl_insert(AVLTree_t *tree, void *data);
void *avl_find(AVLTree_t *tree, void *data);
void *avl_remove(AVLTree_t *tree, void *data);

size_t avl_getSize(const AVLTree_t *tree);

typedef void (*TraverseFunc_t)(void *node_data, void *context);
void avl_foreach(const AVLTree_t *tree, TraverseFunc_t func, void *context);

#endif
