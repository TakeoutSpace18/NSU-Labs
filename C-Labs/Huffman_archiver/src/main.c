#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

#include "Compress.h"
#include "Decompress.h"

void ExitWithError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

bool IsEmptyStream(FILE* stream) {
    int c = fgetc(stream);
    if (c == EOF) return true;
    ungetc(c, stream);
    return false;
}

int main(int argc, char *argv[]) {
    FILE* input = stdin;
    FILE* output = stdout;
    char mode = 0;

    if (argc == 1) {
        mode = getc(input);
    }
    else if (argc == 4) {
        mode = argv[1][0];
        if ((input = fopen(argv[2], "rb")) == NULL) {
            ExitWithError("Can't open file \'%s\'\n", argv[2]);
        }
        if ((output = fopen(argv[3], "wb")) == NULL) {
            ExitWithError("Can't create file \'%s\'\n", argv[3]);
        }
    }
    else ExitWithError("Invalid arguments\n");

    if (IsEmptyStream(input)) {
        exit(EXIT_SUCCESS);
    }

    if (mode == 'c') { // Compress
        Compress(input, output);
    }
    else if (mode == 'd') { // Decompress
        Decompress(input, output);
    }
    else {
        ExitWithError("\'%c\' - unknown mode identifier\n", mode);
    }

    return EXIT_SUCCESS;
}
