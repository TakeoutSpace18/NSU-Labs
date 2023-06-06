#include <stdlib.h>
#include <stdio.h>

#include "error_handling.h"
#include "find_substring.h"

#define SUBSTRING_MAX_SIZE 16

int main(void) {
    exitCode_t exitCode = SUCCESS;

    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    char substring[SUBSTRING_MAX_SIZE + 1];
    if (fscanf(input, "%16[^\n]s", substring) != 1) {
        return_defer(SUCCESS);
    }
    fgetc(input);
    findSubstring(input, output, substring);

defer:
    printExitCodeDescription(stderr, exitCode);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }
    return exitCode;
}
