#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Compress.h"
#include "BitStream.h"
#include "HuffmanTree.h"

void Compress(FILE* in, FILE* out) {
    // save current position in input stream
    fpos_t startPos;
    fgetpos(in, &startPos);

    uint32_t* freqTable = calloc(256, sizeof(int));
    uint16_t uniqueBytesCount = 0;
    uint32_t totalBytesCount = 0;

    for (int16_t byte = getc(in); byte != EOF; byte = getc(in)) {
        if (freqTable[byte] == 0) {
            ++uniqueBytesCount;
        }
        ++freqTable[byte];
        ++totalBytesCount;
    }

    BitStream* bitOutput = OpenBitStream(out);
    WriteByteArray(bitOutput, &totalBytesCount, sizeof(totalBytesCount));

    HTree* huffmanTree = BuildHuffmanTree(freqTable, uniqueBytesCount);
    free(freqTable);

    SerializeHuffmanTree(bitOutput, huffmanTree);

    char** codeTable = MakeCodeTable(huffmanTree);

    // return to saved position and do the compression
    fsetpos(in, &startPos);
    for (int16_t byte = getc(in); byte != EOF; byte = getc(in)) {
        EmitCode(bitOutput, codeTable, byte);
    }

    CloseBitStream(bitOutput);
    DestroyCodeTable(codeTable);
    DestroyHuffmanTree(huffmanTree);
}
