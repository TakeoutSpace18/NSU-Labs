#define _GNU_SOURCE

#include <stdlib.h>
#include <ctype.h>

#include "monogram_analysis.h"
#include "substitution_cipher.h"

const char *englishFrequencyList = "etaoinshrdlcumwfgypbvkjxqz";

static void fillFrequencyTable(FILE *input, size_t *frequencyTable) {
    fpos_t startPos;
    fgetpos(input, &startPos);

    for (int c = fgetc(input); c != EOF; c = fgetc(input)) {
        if (isalpha(c)) {
            ++frequencyTable[tolower(c) - 'a'];
        }
    }
    fsetpos(input, &startPos);
}

static int letterCmp(const void *a, const void *b, void *arg) {
    size_t *frequencyTable = arg;
    size_t freq_a = frequencyTable[*(char*)a - 'a'];
    size_t freq_b = frequencyTable[*(char*)b - 'a'];

    if (freq_a < freq_b) {
        return 1;
    }
    else if (freq_a > freq_b) {
        return -1;
    }
    else {
        return 0;
    }
}

exitCode_t monogramFrequencyAnalysis(FILE *input, char *hackedKey) {
    size_t *frequencyTable = calloc(ALPHABET_SIZE, sizeof(size_t));
    if (!frequencyTable) {
        log_error("calloc() failed");
        return ALLOCATION_ERROR;
    }

    fillFrequencyTable(input, frequencyTable);
    char fileFrequencyList[] = "abcdefghijklmnopqrstuvwxyz";
    qsort_r(fileFrequencyList, ALPHABET_SIZE, sizeof(char), &letterCmp, frequencyTable);

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        hackedKey[englishFrequencyList[i] - 'a'] = fileFrequencyList[i];
    }
    hackedKey[ALPHABET_SIZE] = 0;

    free(frequencyTable);
    return SUCCESS;
}
