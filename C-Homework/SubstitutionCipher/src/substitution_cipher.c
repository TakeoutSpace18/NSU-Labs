#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "substitution_cipher.h"
#include "monogram_analysis.h"
#include "trigram_analysis.h"

static bool validateKey(const char *key) {
    if (strlen(key) != ALPHABET_SIZE) {
        return false;
    }
    for (const char *p = key; *p; ++p) {
        if (!isalpha(*p) || !islower(*p)) {
            return false;
        }
        // check for duplicate letters
        for (const char *p2 = p + 1; *p2; ++p2) {
            if (*p2 == *p) {
                return false;
            }
        }
    }
    return true;
}

static inline char getReplacement(const char *key, char c) {
    if (!isalpha(c)) {
        return c;
    }

    if (isupper(c)) {
        return toupper(key[c - 'A']);
    }
    else {
        return key[c - 'a'];
    }
}

static void replaceLetters(FILE *input, FILE *output, const char *key) {
    for (int c = fgetc(input); c != EOF; c = fgetc(input)) {
        fputc(getReplacement(key, c), output);
    }
}

exitCode_t encode(FILE *input, FILE *output, const char *key) {
    if (!validateKey(key)) {
        log_error("key is not valid");
        return INVALID_ARGUMENTS;
    }

    replaceLetters(input, output, key);

    return SUCCESS;
}

static void makeDecodeKey(const char *key, char *decodeKey) {
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        decodeKey[key[i] - 'a'] = 'a' + i;
    }
}

exitCode_t decode(FILE *input, FILE *output, const char *key) {
    if (!validateKey(key)) {
        log_error("key is not valid");
        return INVALID_ARGUMENTS;
    }

    char decodeKey[ALPHABET_SIZE + 1] = { 0 };
    makeDecodeKey(key, decodeKey);

    replaceLetters(input, output, decodeKey);

    return SUCCESS;
}

exitCode_t hack(FILE *input, char *hackedKey) {
    exitCode_t status = monogramFrequencyAnalysis(input, hackedKey);
    if (status != SUCCESS) {
        return status;
    }

    status = trigramFrequencyAnalysis(input, hackedKey);

    return status;
}
