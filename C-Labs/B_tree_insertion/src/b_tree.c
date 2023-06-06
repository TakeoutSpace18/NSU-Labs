#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>

#include "b_tree.h"

BTree_t *btree_create(size_t half_degree) {
    BTree_t *tree = malloc(sizeof(BTree_t));
    if (!tree) {
        return NULL;
    }
    tree->height = 0;
    tree->root = NULL;
    tree->t = half_degree;
    return tree;
}

static BTreeNode_t *createNode(BTree_t *tree) {
    BTreeNode_t *node = malloc(sizeof(BTreeNode_t));
    if (!node) {
        return NULL;
    }

    size_t t = tree->t;
    node->values = calloc(2*t - 1, sizeof(int));
    if (!node->values) {
        free(node);
        return NULL;
    }
    node->children = calloc(2*t, sizeof(BTreeNode_t*));
    if (!node->children) {
        free(node->values);
        free(node);
        return NULL;
    }
    node->size = 0;
    return node;
}

static void destroyNode(BTreeNode_t **node) {
    if (!node || !(*node)) {
        return;
    }
    free((*node)->values);
    free((*node)->children);
    free(*node);
    *node = NULL;
}

static void destroyTreeNodes(BTreeNode_t *node) {
    if (!node) {
        return;
    }

    for (size_t i = 0; i <= node->size; ++i) {
        destroyTreeNodes(node->children[i]);
    }
    destroyNode(&node);
}

void btree_destroy(BTree_t **tree) {
    if (!tree || !(*tree)) {
        return;
    }
    destroyTreeNodes((*tree)->root);
    free(*tree);
    *tree = NULL;
}

static bool isFull(BTree_t *btree, BTreeNode_t *node) {
    return node->size == (btree->t * 2 - 1);
}

static bool isLeaf(BTreeNode_t *node) {
    return node->children[0] == NULL;
}

static size_t findChildIdx(BTreeNode_t *node, int value) {
    size_t l = 0;
    size_t r = node->size;

    // binary search
    while (l < r) {
        size_t mid = l + (r - l) / 2;
        if (value <= node->values[mid]) {
            r = mid;
        }
        else {
            l = mid + 1;
        }
    }

    return r;
}

static void moveTail(BTreeNode_t *src, size_t src_pos, BTreeNode_t *dest, size_t dest_pos) {
    size_t sz = src->size - src_pos;
    memmove(dest->values + dest_pos, src->values + src_pos, sz * sizeof(int));
    memmove(dest->children + dest_pos, src->children + src_pos, (sz + 1) * sizeof(BTreeNode_t*));
}

static exitCode_t splitChild(BTree_t *tree, BTreeNode_t *node, size_t childIdx) {
    BTreeNode_t *child = node->children[childIdx];
    BTreeNode_t *newChild = createNode(tree);
    if (!newChild) {
        log_error("failed to create node");
        return ALLOCATION_ERROR;
    }

    moveTail(child, tree->t, newChild, 0);
    child->size = tree->t - 1;
    newChild->size = tree->t - 1;

    // insert child's median value to node
    moveTail(node, childIdx, node, childIdx + 1);
    node->values[childIdx] = child->values[tree->t - 1];
    node->children[childIdx + 1] = newChild;
    node->size++;

    return SUCCESS;
}

static exitCode_t splitRoot(BTree_t *tree) {
    BTreeNode_t *oldRoot = tree->root;

    BTreeNode_t *newRoot = createNode(tree);
    if (!newRoot) {
        log_error("failed to create node");
        return ALLOCATION_ERROR;
    }

    BTreeNode_t *newChild = createNode(tree);
    if (!newChild) {
        destroyNode(&newRoot);
        log_error("failed to create node");
        return ALLOCATION_ERROR;
    }

    moveTail(oldRoot, tree->t, newChild, 0);
    newChild->size = tree->t - 1;
    oldRoot->size = tree->t - 1;

    // insert oldRoot's median value to newRoot
    newRoot->values[0] = oldRoot->values[tree->t - 1];
    newRoot->children[0] = oldRoot;
    newRoot->children[1] = newChild;
    newRoot->size = 1;

    tree->height++;
    tree->root = newRoot;

    return SUCCESS;
}

static void insertValue(BTreeNode_t *node, int value) {
    size_t childIdx = findChildIdx(node, value);
    moveTail(node, childIdx, node, childIdx + 1);
    node->values[childIdx] = value;
    node->size++;
}

exitCode_t btree_insert(BTree_t *tree, int value) {
    if (tree->root == NULL) {
        tree->root = createNode(tree);
        if (!tree->root) {
            log_error("failed to create node");
            return ALLOCATION_ERROR;
        }
        tree->root->values[0] = value;
        tree->root->size = 1;
        tree->height = 1;
        return SUCCESS;
    }

    if (isFull(tree, tree->root)) {
        exitCode_t status = splitRoot(tree);
        if (status != SUCCESS) {
            return status;
        }
    }

    BTreeNode_t *curNode = tree->root;

    while (!isLeaf(curNode)) {
        size_t childIdx = findChildIdx(curNode, value);
        if (!curNode->children[childIdx]) {
            curNode->children[childIdx] = createNode(tree);
            if (!curNode->children[childIdx]) {
                return ALLOCATION_ERROR;
            }
        }
        if (isFull(tree, curNode->children[childIdx])) {
            exitCode_t status = splitChild(tree, curNode, childIdx);
            if (status != SUCCESS) {
                return status;
            }
            // choose between splited children
            if (value > curNode->values[childIdx]) {
                childIdx++;
            }
        }
        curNode = curNode->children[childIdx];
    }
    insertValue(curNode, value);

    return SUCCESS;
}

size_t btree_getHeight(BTree_t *tree) {
    return tree->height;
}
