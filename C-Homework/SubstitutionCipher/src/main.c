#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#include <ctype.h>

#include "error_handling.h"
#include "substitution_cipher.h"

static void printManual(FILE *output) {
    fprintf(output, "Usage:\n");
    fprintf(output, "./SubstitutionSipher <in_file> <encode | decode> <key> <out_file>\n");
    fprintf(output, "./SubstitutionSipher <in_file> <hack> <out_file>\n");
}

typedef enum {
    ENCODE,
    DECODE,
    HACK,
    UNKNOWN
} Mode_t;

static Mode_t getMode(const char *mode_string) {
    if (strcmp(mode_string, "encode") == 0) {
        return ENCODE;
    }
    else if (strcmp(mode_string, "decode") == 0) {
        return DECODE;
    }
    else if (strcmp(mode_string, "hack") == 0) {
        return HACK;
    }
    else {
        return UNKNOWN;
    }
}

static void makeLower(char *str) {
    for (char *p = str; *p; ++p) {
        *p = tolower(*p);
    }
}

typedef struct {
    Mode_t mode;
    const char *input_filename;
    const char *output_filename;
    char *key;
} cmdArgs_t;

static exitCode_t parseCmdArguments(cmdArgs_t *dest, int argc, char **argv) {
    if (argc == 4 || argc == 5) {
        dest->input_filename = argv[1];
        dest->mode = getMode(argv[2]);

        if (dest->mode == ENCODE || dest->mode == DECODE) {
            dest->key = argv[3];
            makeLower(dest->key);
            dest->output_filename = argv[4];
        }
        else if (dest->mode == HACK) {
            dest->key = NULL;
            dest->output_filename = argv[3];
        }
        else {
            fprintf(stderr, "Unknown mode\n");
            return INVALID_ARGUMENTS;
        }
    }
    else {
        if (argc == 1) {
            fprintf(stderr, "No arguments passed\n");
        }
        else {
            fprintf(stderr, "Too many arguments passed\n");
        }
        printManual(stderr);
        return INVALID_ARGUMENTS;
    }

    return SUCCESS;
}

int main(int argc, char **argv) {
    exitCode_t exitCode = SUCCESS;

    FILE *input = NULL;
    FILE *output = NULL;

    cmdArgs_t cmdArgs;
    exitCode = parseCmdArguments(&cmdArgs, argc, argv);
    if (exitCode != SUCCESS) {
        return_defer(exitCode);
    }

    input = fopen(cmdArgs.input_filename, "r");
    output = fopen(cmdArgs.output_filename, "w");

    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    switch (cmdArgs.mode) {
    case ENCODE:
        encode(input, output, cmdArgs.key);
        break;

    case DECODE:
        decode(input, output, cmdArgs.key);
        break;

    case HACK: {
        char hackedKey[ALPHABET_SIZE];
        hack(input, hackedKey);
        fprintf(output, "%s\n", hackedKey);
        decode(input, output, hackedKey);
        break;
    }
    default:
        return_defer(INVALID_ARGUMENTS);
        break;
    }

defer:
    printExitCodeDescription(stderr, exitCode);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }

    return EXIT_SUCCESS;
}
