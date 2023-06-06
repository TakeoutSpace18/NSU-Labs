#include "trigram_analysis.h"

#include <ctype.h>
#include <stdlib.h>
#include <memory.h>

#include "substitution_cipher.h"

const char *englishFrequentTrigrams[] = {
    "the",
    "and",
    "ing"
};

#define TRIGRAMS_COUNT sizeof(englishFrequentTrigrams) / sizeof(englishFrequentTrigrams[0])

static int getAlpha(FILE *input) {
    int c = fgetc(input);
    while (c != EOF && !isalpha(c)) {
        c = fgetc(input);
    }
    return c;
}

static void fillTrigramFrequencies(FILE *input, size_t ***trigramFrequencies) {
    fpos_t startPos;
    fgetpos(input, &startPos);

    int trigram[3];
    trigram[0] = tolower(getAlpha(input));
    trigram[1] = tolower(getAlpha(input));

    if (trigram[1] == EOF) {
        return;
    }

    for (int c = getAlpha(input); c != EOF; c = getAlpha(input)) {
        trigram[2] = tolower(c);

        int i = trigram[0] - 'a';
        int j = trigram[1] - 'a';
        int k = trigram[2] - 'a';

        ++trigramFrequencies[i][j][k];

        trigram[0] = trigram[1];
        trigram[1] = trigram[2];
    }

    fsetpos(input, &startPos);
}

static void getMostFrequentTrigrams(size_t ***trigramFrequencies, char mostFrequent[TRIGRAMS_COUNT][3]) {
    size_t maxFrequencies[TRIGRAMS_COUNT] = {0};

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        for (int j = 0; j < ALPHABET_SIZE; ++j) {
            for (int k = 0; k < ALPHABET_SIZE; ++k) {

                char curTrigram[3] = {'a' + i, 'a' + j, 'a' + k};
                size_t curTrigramFrequency = trigramFrequencies[i][j][k];

                for (size_t pos = 0; pos < TRIGRAMS_COUNT; ++pos) {
                    if (curTrigramFrequency > maxFrequencies[pos]) {
                        memmove(maxFrequencies + pos + 1, maxFrequencies + pos, (TRIGRAMS_COUNT - pos - 1) * sizeof(size_t));
                        maxFrequencies[pos] = curTrigramFrequency;

                        memmove(mostFrequent + pos + 1, mostFrequent + pos, (TRIGRAMS_COUNT - pos - 1) * sizeof(char[3]));
                        memcpy(mostFrequent[pos], curTrigram, 3 * sizeof(char));
                        break;
                    }
                }
            }
        }
    }
}

static void destroyTrigramsFreqTable(size_t ***table) {
    if (!table) {
        return;
    }
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (table[i]) {
            for (int j = 0; j < ALPHABET_SIZE; ++j) {
                free(table[i][j]);
            }
            free(table[i]);
        }
    }
    free(table);
}

static size_t ***createTrigramsFreqTable() {
    size_t ***table = calloc(ALPHABET_SIZE, sizeof(size_t**));
    if (!table) {
        return NULL;
    }
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        table[i] = calloc(ALPHABET_SIZE, sizeof(size_t*));
        if (!table[i]) {
            destroyTrigramsFreqTable(table);
            return NULL;
        }
        for (int j = 0; j < ALPHABET_SIZE; ++j) {
            table[i][j] = calloc(ALPHABET_SIZE, sizeof(size_t));
            if (!table[i][j]) {
                destroyTrigramsFreqTable(table);
                return NULL;
            }
        }
    }
    return table;
}

static void replaceTrigramInKey(char *key, const char trigram_a[3], const char trigram_b[3]) {
    for (int letter = 0; letter < 3; ++letter) {
        char tmp = key[trigram_a[letter] - 'a'];
        for (int i = 0; i < ALPHABET_SIZE; ++i) {
            if (key[i] == trigram_b[letter]) {
                key[i] = tmp;
            }
        }
        key[trigram_a[letter] - 'a'] = trigram_b[letter];
    }
}

exitCode_t trigramFrequencyAnalysis(FILE *input, char *hackedKey) {
    size_t ***trigramFrequencies = createTrigramsFreqTable();
    if (!trigramFrequencies) {
        return ALLOCATION_ERROR;
    }

    fillTrigramFrequencies(input, trigramFrequencies);

    char mostFrequent[TRIGRAMS_COUNT][3];
    getMostFrequentTrigrams(trigramFrequencies, mostFrequent);

    for (int trig = TRIGRAMS_COUNT - 1; trig >= 0; --trig) {
        replaceTrigramInKey(hackedKey, englishFrequentTrigrams[trig], mostFrequent[trig]);
    }

    destroyTrigramsFreqTable(trigramFrequencies);
    return SUCCESS;
}
