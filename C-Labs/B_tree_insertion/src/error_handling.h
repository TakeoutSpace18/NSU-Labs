#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <stdio.h>

#define log_error(msg) setErrorMessage("%s %s: " msg, __FILE__, __func__)

#define return_defer(value) do { \
    exitCode = (value); \
    goto defer; \
} while (0)

typedef enum exitCode {
    SUCCESS = 0,
    ALLOCATION_ERROR,
    IO_ERROR,
    WRONG_INPUT
} exitCode_t;

void setErrorMessage(const char* msg, ...);
void printExitCodeDescription(FILE *stream, exitCode_t exitCode);

#endif
