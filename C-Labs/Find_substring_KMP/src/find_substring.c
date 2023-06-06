#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "find_substring.h"
#include "error_handling.h"

#define BUFFER_SIZE 1024

typedef struct SearchState {
    char *substring;
    int *prefix;
    size_t substrLen;
    size_t matchCount;
} SearchState_t;

typedef struct SearchWindow {
    FILE *inputStream;
    char textBuffer[BUFFER_SIZE + 1];
    size_t textLen;
    size_t curIndex;
    size_t globalIndex;
    int isEmpty;
} SearchWindow_t;

static void initPrefixTable(int *prefix, const char *substring, size_t substrLen) {
    prefix[0] = 0;
    for (size_t i = 1; i < substrLen; ++i) {
        size_t k = prefix[i - 1];
        while(k > 0 && substring[i] != substring[k]) {
            k = prefix[k - 1];
        }
        if (substring[i] == substring[k]) {
            ++k;
        }
        prefix[i] = k;
    }
}

static void printPrefixTable(FILE *output, const SearchState_t *state) {
    for (size_t i = 0; i < state->substrLen; ++i) {
        fprintf(output, "%i ", state->prefix[i]);
    }
    fprintf(output, "\n");
}

static void destroySearchState(SearchState_t **st) {
    if (!st || !(*st)) {
        return;
    }
    free((*st)->substring);
    free((*st)->prefix);
    free(*st);
    *st = NULL;
}

static SearchState_t *initSearchState(const char *substring) {
    exitCode_t exitCode = SUCCESS;

    SearchState_t *st = malloc(sizeof(SearchState_t));
    if (!st) {
        return_defer(ALLOCATION_ERROR);
    }

    st->substrLen = strlen(substring);
    st->matchCount = 0;

    st->substring = malloc((st->substrLen + 1) * sizeof(char));
    if (!st->substring) {
        return_defer(ALLOCATION_ERROR);
    }
    strcpy(st->substring, substring);

    st->prefix = malloc((st->substrLen) * sizeof(int));
    if (!st->prefix) {
        return_defer(ALLOCATION_ERROR);
    }
    initPrefixTable(st->prefix, st->substring, st->substrLen);

defer:
    if (exitCode != SUCCESS) {
        destroySearchState(&st);
        return NULL;
    }

    return st;
}

static SearchWindow_t *initSearchWindow(FILE *input, size_t substrLen) {
    SearchWindow_t* wnd = malloc(sizeof(SearchWindow_t));
    if (!wnd) {
        return NULL;
    }

    wnd->inputStream = input;
    wnd->curIndex = 0;
    wnd->globalIndex = 0;

    // read initial part of the text and check if it is not empty
    wnd->textLen = fread(wnd->textBuffer, sizeof(*wnd->textBuffer), BUFFER_SIZE, wnd->inputStream);
    wnd->isEmpty = wnd->textLen < substrLen ? 1 : 0;

    return wnd;
}

static bool isMatch(const SearchWindow_t *window, SearchState_t *state) {
    for (size_t pos = state->matchCount; pos < state->substrLen; ++pos) {
        if (window->textBuffer[window->curIndex + pos] != state->substring[pos]) {
            state->matchCount = pos;
            return false;
        };
    }
    state->matchCount = state->substrLen;
    return true;
}

static int isEmpty(const SearchWindow_t *window) {
    return window->isEmpty;
}

static size_t calculateShift(SearchState_t *state) {
    size_t matchCount = state->matchCount;
    if (matchCount == 0) {
        return 1;
    }

    int prefix = state->prefix[matchCount - 1];
    state->matchCount = prefix;

    return matchCount - prefix;
}

static void moveForward(SearchWindow_t *window, SearchState_t *state) {
    size_t shift = calculateShift(state);
    window->curIndex += shift;
    window->globalIndex += shift;

    // read new chunk of text if the end of the buffer is reached
    if (window->curIndex + state->substrLen > BUFFER_SIZE) {

        // copy remained part of the old text to the begining
        size_t remainedTextLen = BUFFER_SIZE - window->curIndex;
        memmove(window->textBuffer, window->textBuffer + window->curIndex, remainedTextLen);

        // append it with new text
        char* whereToRead = window->textBuffer + remainedTextLen;
        size_t howManyToRead = BUFFER_SIZE - remainedTextLen;
        size_t symbolsRead = fread(whereToRead, sizeof(*whereToRead), howManyToRead, window->inputStream);

        window->textLen = remainedTextLen + symbolsRead;
        window->curIndex = 0;
    }

    // check if the end of whole text is reached
    if (window->curIndex + state->substrLen > window->textLen) {
        window->isEmpty = 1;
    }
}

void findSubstring(FILE *input, FILE *output, const char* substring) {
    SearchState_t *state = initSearchState(substring);
    SearchWindow_t *window = initSearchWindow(input, strlen(substring));
    if (!window || !state) {
        log_error("init failed");
        goto defer;
    }

    printPrefixTable(output, state);

    while (!isEmpty(window)) {
        if (isMatch(window, state)) {
            // do smth if substring is found
        }

        if (state->matchCount != 0) {
            fprintf(output, "%zu ", window->globalIndex + 1);
            fprintf(output, "%zu ", state->matchCount);
        }

        moveForward(window, state);
    }

defer:
    free(window);
    destroySearchState(&state);
}
