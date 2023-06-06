#pragma once
#include <stdint.h>

#include "BitStream.h"

typedef struct HTree {
struct HTree* left;
    struct HTree* right;
    uint8_t value;
} HTree;

typedef struct HSortNode {
    HTree* treeNode;
    size_t frequency;
} HSortNode;

HTree* BuildHuffmanTree(uint32_t* freqTable, uint16_t uniqueBytesCount);
void DestroyHuffmanTree(HTree* tree);

void SerializeHuffmanTree(BitStream* bitOut, HTree* tree);
HTree* DeserializeHuffmanTree(BitStream* bitInput);

char** MakeCodeTable(HTree* tree);
void DestroyCodeTable(char** codeTable);
void EmitCode(BitStream* bitOut, char** codeTable, uint8_t byte);

int16_t DecodeByte(BitStream* bitIn, HTree* tree);
