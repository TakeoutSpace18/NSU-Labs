#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>

#define log_error(msg) do { setErrorMessage("%s %s: " msg, __FILE__, __func__); } while (0)

#define return_defer(value) do { \
    exitCode = (value); \
    goto defer; \
} while (0)

typedef enum exitCode {
    SUCCESS = 0,
    ALLOCATION_ERROR,
    IO_ERROR,
    BAD_NUMBER_OF_VERTICES,
    BAD_NUMBER_OF_EDGES,
    BAD_NUMBER_OF_LINES,
    BAD_VERTEX,
    BAD_LENGTH,
    NO_SPANNING_TREE,
    OUT_OF_BOUNDS,
    INVALID_ARGUMENT
} exitCode_t;

void setErrorMessage(const char* msg, ...);
void printExitCodeDescription(FILE *stream, exitCode_t exitCode);

#endif
