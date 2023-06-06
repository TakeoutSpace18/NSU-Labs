#ifndef CRYPTO_ANALYSIS_H
#define CRYPTO_ANALYSIS_H

#include "error_handling.h"

#define ALPHABET_SIZE 26

exitCode_t encode(FILE *input, FILE *output, const char *key);
exitCode_t decode(FILE *input, FILE *output, const char *key);
exitCode_t hack(FILE *input, char *hackedKey);

#endif
