#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef char T;

int badInput() {
    printf("bad input\n");
    exit(0);
}

int hasRepeatedSymbols(char* str) {
    for (char* p1 = str; *p1; ++p1) {
        for (char* p2 = str; p2 < p1; ++p2) {
            if (*p2 == *p1) return 1;
        }
    }
    return 0;
}

void validateSequence(char* seq) {
    for (char* p = seq; *p; ++p) {
        if (*p < '0' || *p > '9') {
            badInput();
        }
    }
    if (hasRepeatedSymbols(seq)) {
        badInput();
    }
}

void swap(T* a, T* b) {
    T tmp = *a;
    *a = *b;
    *b = tmp;
}

void reverse(T* start, T* end) {
    while (start < end) {
        swap(start, end);
        ++start;
        --end;
    }
}

int makeNextPermutation(T* sequence, size_t sz) {
    T* start = sequence;
    T* end = start + sz - 1;

    // one-element sequence => no permutations
    if (start == end) return -1;

    // find first ascending pair,
    // starting from the end of the sequence
    T* p = end - 1;
    while (*p >= *(p + 1)) {
        --p;
        if (p < start) {
            return -1; // no pair found
            // => no greater permutation
        }
    }

    // find lowest element, greater than *p and swap them
    T* lo = end;
    while (*lo <= *p) --lo;
    swap(p, lo);

    // reverse tail of the sequence
    reverse(p + 1, end);
    return 1;
}

int main(void) {
    char sequence[51];
    int permutationsAmount;

    // read whole line until '/n'
    if (1 != scanf("%50[^\n]s", sequence)) badInput();
    if (1 != scanf("%i", &permutationsAmount)) badInput();

    validateSequence(sequence);

    size_t sequenceLen = strlen(sequence);
    for (int i = 0; i < permutationsAmount; ++i) {
        if (makeNextPermutation(sequence, sequenceLen) == -1) {
            break;
        }
        printf("%s\n", sequence);
    }

    return EXIT_SUCCESS;
}
