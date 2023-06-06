#ifndef B_TREE_H
#define B_TREE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "error_handling.h"

typedef struct BTreeNode {
    int *values;
    struct BTreeNode **children;
    size_t size;
} BTreeNode_t;

typedef struct BTree {
    BTreeNode_t *root;
    size_t t;
    size_t height;
} BTree_t;

BTree_t *btree_create(size_t half_degree);
void btree_destroy(BTree_t **tree);

exitCode_t btree_insert(BTree_t *tree, int value);

size_t btree_getHeight(BTree_t *tree);

#endif
