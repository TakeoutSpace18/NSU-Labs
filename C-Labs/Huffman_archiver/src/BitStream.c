#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "BitStream.h"

BitStream* OpenBitStream(FILE* file) {
    BitStream* s = malloc(sizeof(BitStream));
    s->file = file;
    s->buffer = 0;
    s->bufferedBitsCount = 0;
    return s;
}

static void Flush(BitStream* s) {
    // if buffer is not full, zeros will be flushed to fill remaining space.
    if (s->bufferedBitsCount != 0) {
        fwrite(&s->buffer, sizeof(s->buffer), 1, s->file);
        s->buffer = 0;
        s->bufferedBitsCount = 0;
    }
}

void CloseBitStream(BitStream* s) {
    Flush(s);
    free(s);
}

void WriteBit(BitStream* s, bool bit) {
    const int bufferCapacity = sizeof(s->buffer) * CHAR_BIT;
    s->buffer |= (bit << (bufferCapacity - s->bufferedBitsCount - 1));
    ++s->bufferedBitsCount;

    if (s->bufferedBitsCount == bufferCapacity) {
        Flush(s);
    }
}

void WriteByte(BitStream* s, uint8_t byte) {
    for(int i = 0; i < 8; ++i) {
        WriteBit(s, byte & (0x1 << i));
    }
}

void WriteByteArray(BitStream* s, void* array, size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        WriteByte(s, ((uint8_t*)array)[i]);
    }
}

int16_t ReadBit(BitStream* s) {
    const int bufferCapacity = sizeof(s->buffer) * CHAR_BIT;
    if (s->bufferedBitsCount == 0) {
        int success = fread(&s->buffer, sizeof(s->buffer), 1, s->file);
        if (!success) return EOF;
        s->bufferedBitsCount = bufferCapacity;
    }
    return !!(s->buffer & (0x1 << (--s->bufferedBitsCount)));

}

int16_t ReadByte(BitStream* s) {
    uint8_t byte = 0;
    for (int i = 0; i < 8; ++i) {
        int bit = ReadBit(s);
        if (bit == EOF){
            return EOF;
        }
        byte |= (bit << i);
    }
    return byte;
}

void ReadByteArray(BitStream* s, void* dest, size_t sz) {
    for (size_t i = 0; i < sz; ++i) {
        ((uint8_t*)dest)[i] = ReadByte(s);
    }
}
