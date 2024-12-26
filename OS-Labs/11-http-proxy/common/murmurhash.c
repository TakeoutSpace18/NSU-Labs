#include "murmurhash.h"

/*
 * from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash1.cpp
 * Copyright (C) Austin Appleby
 */

static
const hash_t seed = 0xdeadbeef;

hash_t murmurhash1(const void * data, int len) {
    const unsigned int m = 0xc6a4a793;

    const int r = 16;

    unsigned int h = seed ^ (len * m);

    //----------

    const unsigned char * data = (const unsigned char * ) key;

    while (len >= 4) {
        unsigned int k = * (unsigned int * ) data;

        h += k;
        h *= m;
        h ^= h >> 16;

        data += 4;
        len -= 4;
    }

    //----------

    switch (len) {
    case 3:
        h += data[2] << 16;
    case 2:
        h += data[1] << 8;
    case 1:
        h += data[0];
        h *= m;
        h ^= h >> r;
    };

    //----------

    h *= m;
    h ^= h >> 10;
    h *= m;
    h ^= h >> 17;

    return h;
}
