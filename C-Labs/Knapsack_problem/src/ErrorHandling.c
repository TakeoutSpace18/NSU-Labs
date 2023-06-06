#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "ErrorHandling.h"

void PrintAndExitWithError(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

void ExitWithError(ErrorType error) {
    switch (error) {
    case ALLOCATION_ERROR:
        PrintAndExitWithError("Allocation failed\n");
        break;
        
    case INPUT_ERROR:
        PrintAndExitWithError("Wrong input\n");
        break;

    default:
        PrintAndExitWithError("Unknown error\n");
        break;
    }
}
