#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct BitStream {
    FILE *file;
    uint8_t buffer;
    uint8_t bufferedBitsCount;
} BitStream;

BitStream* OpenBitStream(FILE* file);
void CloseBitStream(BitStream* stream);

void WriteBit(BitStream* stream, bool bit);
void WriteByte(BitStream* stream, uint8_t byte);
void WriteByteArray(BitStream* stream, void* array, size_t sz);

int16_t ReadBit(BitStream* stream);
int16_t ReadByte(BitStream* stream);
void ReadByteArray(BitStream* stream, void* dest, size_t sz);
