#include "Decompress.h"
#include "BitStream.h"
#include "HuffmanTree.h"

void Decompress(FILE* in, FILE* out) {
    BitStream* bitInput = OpenBitStream(in);

    uint32_t totalBytesEncoded;
    ReadByteArray(bitInput, &totalBytesEncoded, sizeof(totalBytesEncoded));

    HTree* huffmanTree = DeserializeHuffmanTree(bitInput);

    for (size_t bytesDecoded = 0; bytesDecoded < totalBytesEncoded; ++bytesDecoded) {
        int16_t byte = DecodeByte(bitInput, huffmanTree);
        if (byte == EOF) {
            break;
        }
        fwrite(&byte, 1, 1, out);
    }

    DestroyHuffmanTree(huffmanTree);
    CloseBitStream(bitInput);
}
