#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "HuffmanTree.h"
#include "Queue.h"

static HTree* CreateTreeNode(uint8_t value) {
    HTree* node = malloc(sizeof(HTree));
    node->left = NULL;
    node->right = NULL;
    node->value = value;
    return node;
}

static bool IsLeafNode(HTree* node) {
    return node->left == NULL && node->right == NULL;
}

static int HSortNodeCompare(const void* a, const void* b) {
    HSortNode* nodeA = (HSortNode*)a;
    HSortNode* nodeB = (HSortNode*)b;
    return nodeA->frequency > nodeB->frequency;
}

static Queue* PrepareLeafNodesQueue(uint32_t* freqTable, uint16_t uniqueBytesCount) {
    HSortNode* leaves = malloc(sizeof(HSortNode) * uniqueBytesCount);

    // convert from frequency table to array of HSortNode
    int curIdx = 0;
    for (int i = 0; i < 256; ++i) {
        if (freqTable[i] == 0) continue;
        leaves[curIdx].frequency = freqTable[i];
        leaves[curIdx].treeNode = CreateTreeNode(i);
        curIdx++;
    }

    // sort ascending
    qsort(leaves, uniqueBytesCount, sizeof(leaves[0]), HSortNodeCompare);

    // move to queue
    Queue* leafNodes = CreateQueue();
    for (int i = 0; i < uniqueBytesCount; ++i) {
        Push(leafNodes, leaves[i]);
    }
    free(leaves);
    return leafNodes;
}

static HSortNode PopMinimum(Queue* leafNodes, Queue* compositeNodes) {
    assert(!IsEmpty(leafNodes) || !IsEmpty(compositeNodes));
    if (IsEmpty(leafNodes))
        return Pop(compositeNodes);
    if (IsEmpty(compositeNodes))
        return Pop(leafNodes);

    HSortNode* leaf = GetFront(leafNodes);
    HSortNode* composite = GetFront(compositeNodes);
    if (leaf->frequency < composite->frequency) {
        return Pop(leafNodes);
    }
    else {
        return Pop(compositeNodes);
    }
}

HTree* BuildHuffmanTree(uint32_t* freqTable, uint16_t uniqueBytesCount) {
    assert(freqTable != NULL);
    assert(uniqueBytesCount != 0);

    Queue* leafNodes = PrepareLeafNodesQueue(freqTable, uniqueBytesCount);

    if (GetSize(leafNodes) == 1) {
        HTree* root = Pop(leafNodes).treeNode;
        DestroyQueue(leafNodes);
        return root;
    }

    Queue* compositeNodes = CreateQueue();

    while (GetSize(leafNodes) + GetSize(compositeNodes) > 1) {
        HSortNode m1 = PopMinimum(leafNodes, compositeNodes);
        HSortNode m2 = PopMinimum(leafNodes, compositeNodes);

        HTree* newTreeNode = CreateTreeNode(0);
        newTreeNode->right = m1.treeNode;
        newTreeNode->left = m2.treeNode;

        HSortNode newSortNode;
        newSortNode.frequency = m1.frequency + m2.frequency;
        newSortNode.treeNode = newTreeNode;

        Push(compositeNodes, newSortNode);
    }

    HTree* root = GetFront(compositeNodes)->treeNode;
    DestroyQueue(leafNodes);
    DestroyQueue(compositeNodes);

    return root;
}


void SerializeHuffmanTree(BitStream* bitOut, HTree* tree) {
    assert(tree != NULL);
    if (IsLeafNode(tree)) {
        WriteBit(bitOut, 0x1);
        WriteByte(bitOut, tree->value);
    }
    else {
        WriteBit(bitOut, 0x0);
        SerializeHuffmanTree(bitOut, tree->left);
        SerializeHuffmanTree(bitOut, tree->right);
    }
}

HTree* DeserializeHuffmanTree(BitStream* bitIn) {
    bool isLeafNode = ReadBit(bitIn);
    if (isLeafNode) {
        uint8_t value = ReadByte(bitIn);
        return CreateTreeNode(value);
    }
    else {
        HTree* node = CreateTreeNode(0);
        node->left = DeserializeHuffmanTree(bitIn);
        node->right = DeserializeHuffmanTree(bitIn);
        return node;
    }
}

void DestroyHuffmanTree(HTree* tree) {
    if (tree == NULL) return;
    DestroyHuffmanTree(tree->left);
    DestroyHuffmanTree(tree->right);
    free(tree);
}


// ---------- Code Table ---------- //

static void MakeCodeTableImpl(HTree* node, char** table, char* curCode, int curDepth);

char** MakeCodeTable(HTree* tree) {
    char** table = calloc(256, sizeof(char*));
    char* curCode = malloc(256 * sizeof(char));
    MakeCodeTableImpl(tree, table, curCode, 0);
    free(curCode);
    return table;
}

static void MakeCodeTableImpl(HTree* node, char** table, char* curCode, int curDepth) {
    if (IsLeafNode(node)) {
        curCode[curDepth] = '\0';
        table[node->value] = malloc(256 * sizeof(char));
        strcpy(table[node->value], curCode);
    }
    else {
        curCode[curDepth] = '0';
        MakeCodeTableImpl(node->left, table, curCode, curDepth + 1);

        curCode[curDepth] = '1';
        MakeCodeTableImpl(node->right, table, curCode, curDepth + 1);
    }

}

void DestroyCodeTable(char** codeTable) {
    for (int i = 0; i < 256; ++i) {
        free(codeTable[i]);
    }
    free(codeTable);
}

void EmitCode(BitStream* bitOut, char** codeTable, uint8_t byte) {
    char* code = codeTable[byte];
    for (int i = 0; code[i]; ++i) {
        assert(code[i] == '0' || code[i] == '1');
        switch (code[i]) {
        case '0':
            WriteBit(bitOut, 0x0);
            break;
        case '1':
            WriteBit(bitOut, 0x1);
            break;
        }
    }
}


// ---------- Decoding ---------- //

int16_t DecodeByte(BitStream* bitIn, HTree* tree) {
    HTree* curNode = tree;
    while (!IsLeafNode(curNode)) {
        int16_t bit = ReadBit(bitIn);
        switch (bit) {
        case 0:
            curNode = curNode->left;
            break;
        case 1:
            curNode = curNode->right;
            break;
        case EOF:
            return EOF;
            break;
        }
    }
    return curNode->value;
}
