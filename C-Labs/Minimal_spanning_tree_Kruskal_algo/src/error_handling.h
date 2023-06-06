#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>

typedef enum exitCode {
    SUCCESS = 0,
    ALLOCATION_ERROR,
    INPUT_ERROR,
    BAD_NUMBER_OF_VERTICES,
    BAD_NUMBER_OF_EDGES,
    BAD_NUMBER_OF_LINES,
    BAD_VERTEX,
    BAD_LENGTH,
    NO_SPANNING_TREE,
    OUT_OF_RANGE,
    INVALID_ARGUMENT
} exitCode_t;

void printExitCodeDescription(FILE *stream, exitCode_t exitCode);

#endif
