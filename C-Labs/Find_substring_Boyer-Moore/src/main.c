#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>

typedef unsigned char uchar_t;

#define SUBSTRING_MAX_SIZE 16
#define BUFFER_SIZE 64

void exitWithError() {
    printf("Error\n");
    exit(1);
}

typedef struct SearchState {
    char substring[SUBSTRING_MAX_SIZE + 1];
    size_t substrLen;
    size_t shiftTable[UCHAR_MAX + 1];
    size_t shift;
} SearchState;

void initShiftTable(size_t* table, char* substring, size_t substrLen) {
    for (int i = 0; i <= UCHAR_MAX; ++i) {
        table[i] = substrLen;
    }
    for (int pos = substrLen - 2; pos >= 0; --pos) {
        if (table[(uchar_t)substring[pos]] == substrLen) {
            table[(uchar_t)substring[pos]] = substrLen - pos - 1;
        }
    }
}

SearchState* initSearchState(const char* substring) {
    SearchState* state = malloc(sizeof(SearchState));

    strcpy(state->substring, substring);
    state->substrLen = strlen(substring);

    initShiftTable(state->shiftTable, state->substring, state->substrLen);

    return state;
}

typedef struct SearchWindow {
    FILE* inputStream;
    char textBuffer[BUFFER_SIZE + 1];
    size_t textLen;
    size_t curIndex;
    size_t globalIndex;
    int isEmpty;
} SearchWindow;

SearchWindow* initSearchWindow(FILE* stream) {
    SearchWindow* wnd = malloc(sizeof(SearchWindow));

    wnd->inputStream = stream;
    wnd->curIndex = 0;
    wnd->globalIndex = 0;

    // read initial part of the text and check if it is not empty
    wnd->textLen = fread(wnd->textBuffer, sizeof(*wnd->textBuffer), BUFFER_SIZE, wnd->inputStream);
    wnd->isEmpty = wnd->textLen > 0 ?  0 : 1;

    return wnd;
}

int isMatch(const SearchWindow* window, SearchState* state) {
    for (int64_t pos = state->substrLen - 1; pos >= 0; --pos) {
        // print compared indices
        printf("%zu ", window->globalIndex + pos + 1);
        if (window->textBuffer[window->curIndex + pos] != state->substring[pos]) {
            return 0;
        };
    }
    return 1;
}

int isEmpty(const SearchWindow* window) {
    return window->isEmpty;
}

size_t calculateShift(const SearchWindow* window, const SearchState* state) {
    uchar_t textBack = (uchar_t)window->textBuffer[window->curIndex + state->substrLen - 1];
    return state->shiftTable[textBack];
}

void moveForward(SearchWindow* window, const SearchState* state) {
    size_t shift = calculateShift(window, state);
    window->curIndex += shift;
    window->globalIndex += shift;

    // read new chunk of text if the end of the buffer is reached
    if (window->curIndex + state->substrLen > BUFFER_SIZE) {

        // copy remained part of the old text to the begining
        strcpy(window->textBuffer, &window->textBuffer[window->curIndex]);
        size_t remainedTextLen = BUFFER_SIZE - window->curIndex;

        // append it with new text
        char* whereToRead = &window->textBuffer[remainedTextLen];
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

void findSubstring(FILE* stream, const char* substring) {
    SearchState* state = initSearchState(substring);
    SearchWindow* window = initSearchWindow(stream);

    while (!isEmpty(window)) {
        if (isMatch(window, state)) {
            // do smth if substring is found
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
