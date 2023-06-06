#include <stdio.h>
#include <string.h>

#include "../BitStream.h"

int main(void) {
    FILE* out = fopen("testfile.bin", "wb");
    BitStream* cout = OpenBitStream(out);
    // WriteBit(cout, 1);
    // WriteBit(cout, 0);
    // WriteBit(cout, 1);
    // WriteBit(cout, 1);
    // WriteBit(cout, 0);
    // WriteBit(cout, 1);
    // WriteBit(cout, 0);
    // WriteBit(cout, 1);
    // WriteByte(cout, 0xb5);

    uint64_t val = 212212212212;
    WriteByteArray(cout, &val, sizeof(val));

    CloseBitStream(cout);
    fclose(out);

    FILE* in = fopen("testfile.bin", "rb");
    BitStream* cin = OpenBitStream(in);

    uint64_t val2;
    ReadByteArray(cin, &val2, sizeof(val2));
    printf("%li", val2);

    CloseBitStream(cin);
    fclose(in);

    return 0;
}
