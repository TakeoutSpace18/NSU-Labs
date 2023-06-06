#include "error_handling.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static char errorMessage[200] = {0};

void setErrorMessage(const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vsprintf(errorMessage, msg, args);
    va_end(args);
}

void printExitCodeDescription(FILE *stream, exitCode_t exitCode) {
    if (errorMessage[0]) {
        fprintf(stream, "%s\n", errorMessage);
    }

    switch (exitCode)
    {
    case ALLOCATION_ERROR:
        perror("Allocation error");
        break;
    case IO_ERROR:
        perror("input/output error");
        break;
    case BAD_NUMBER_OF_VERTICES:
        fprintf(stream, "bad number of vertices\n");
        break;
    case BAD_NUMBER_OF_EDGES:
        fprintf(stream, "bad number of edges\n");
        break;
    case BAD_NUMBER_OF_LINES:
        fprintf(stream, "bad number of lines\n");
        break;
    case BAD_VERTEX:
        fprintf(stream, "bad vertex\n");
        break;
    case BAD_LENGTH:
        fprintf(stream, "bad length\n");
        break;
    default:
        break;
    }
}
