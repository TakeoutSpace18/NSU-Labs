#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <locale.h>

typedef unsigned char uchar_t;

#define SUBSTRING_MAX_SIZE 16
#define BUFFER_SIZE 1024

void exitWithError() {
    printf("Error\n");
    exit(1);
}

int hash(const char* str, size_t sz) {
    int sum = 0;
    for (size_t i = 0; i < sz; ++i) {
        sum += ((uchar_t)str[i] % 3) * pow(3, i);
    }
    return sum;
}

typedef struct SearchState {
    char substring[SUBSTRING_MAX_SIZE + 1];
    size_t substrLen;
    int hash;
    int hashMultiplyer; // is used in updateHash
} SearchState;

SearchState* initSearchState(const char* substring) {
    SearchState* state = malloc(sizeof(SearchState));

    strcpy(state->substring, substring);
    state->substrLen = strlen(substring);

    state->hash = hash(substring, state->substrLen);
    state->hashMultiplyer = pow(3, state->substrLen - 1);
    fprintf(stdout, "%i ", state->hash);

    return state;
}

typedef struct SearchWindow {
    FILE* inputStream;
    char textBuffer[BUFFER_SIZE + 1];
    size_t textLen;
    size_t curIndex;
    size_t globalIndex;
    int isEmpty;
    int curHash;
} SearchWindow;

SearchWindow* initSearchWindow(FILE* stream, const SearchState* state) {
    SearchWindow* wnd = malloc(sizeof(SearchWindow));

    wnd->inputStream = stream;
    wnd->curIndex = 0;
    wnd->globalIndex = 0;

    // read initial part of the text, check if it is not empty and calculate hash for it
    wnd->textLen = fread(wnd->textBuffer, sizeof(*wnd->textBuffer), BUFFER_SIZE, wnd->inputStream);
    wnd->isEmpty = wnd->textLen > 0 ?  0 : 1;
    wnd->curHash = hash(wnd->textBuffer, state->substrLen);

    return wnd;
}

int isMatch(const SearchWindow* window, const SearchState* state) {
    if (window->curHash != state->hash) return 0;

    // if hashes don't match, compare strings symbol by symbol
    for (size_t i = 0; i < state->substrLen; ++i) {
        // print compared indices
        printf("%zu ", window->globalIndex + i + 1);
        if (state->substring[i] != window->textBuffer[window->curIndex + i]) return 0;
    }
    return 1;
}

int isEmpty(const SearchWindow* window) {
    return window->isEmpty;
}

void updateHash(SearchWindow* window, const SearchState* state) {
    window->curHash -= (uchar_t)window->textBuffer[window->curIndex - 1] % 3;
    window->curHash /= 3;
    size_t nextCharIdx = window->curIndex + state->substrLen - 1;
    window->curHash += (uchar_t)window->textBuffer[nextCharIdx] % 3 * state->hashMultiplyer;
}

void moveForward(SearchWindow* window, const SearchState* state) {
    // shift searchWindow by 1
    ++window->curIndex;
    ++window->globalIndex;

    // read new chunk of text if the end of the buffer is reached
    if (window->curIndex + state->substrLen > BUFFER_SIZE) {

        // copy remained part of the old text to the begining
        strcpy(window->textBuffer, &window->textBuffer[window->curIndex - 1]);
        size_t remainedTextLen = BUFFER_SIZE - window->curIndex + 1;

        // append it with new text
        char* whereToRead = &window->textBuffer[remainedTextLen];
        size_t howManyToRead = BUFFER_SIZE - remainedTextLen;
        size_t symbolsRead = fread(whereToRead, sizeof(*whereToRead), howManyToRead, window->inputStream);

        window->textLen = remainedTextLen + symbolsRead;
        window->curIndex = 1;
    }

    // check if the end of whole text is reached
    if (window->curIndex + state->substrLen > window->textLen) {
        window->isEmpty = 1;
    }

    updateHash(window, state);
}

void findSubstring(FILE* stream, const char* substring) {
    SearchState* state = initSearchState(substring);
    SearchWindow* window = initSearchWindow(stream, state);

    while (!isEmpty(window)) {
        if (isMatch(window, state)) {
            // do smth if substring found
        }
        moveForward(window, state);
    }
    free(window);
    free(state);
}


int main(int argc, char** argv) {
    FILE *input = stdin;
    if (argc > 1) {
        input = fopen(argv[1], "r");
    }
    char substring[SUBSTRING_MAX_SIZE + 1];
    if (1 != fscanf(input, "%16[^\n]s", substring)) {
        exitWithError();
    }
    fgetc(input);
    findSubstring(input, substring);
    fclose(input);
    return EXIT_SUCCESS;
}
