#ifndef MURMURHASH_H
#define MURMURHASH_H
/*
 * from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash1.cpp
 * Copyright (C) Austin Appleby
 */

#include <stdint.h>

typedef uint32_t hash_t;

hash_t murmurhash1 (const void *data, int len);

#endif /* !MURMURHASH_H */
