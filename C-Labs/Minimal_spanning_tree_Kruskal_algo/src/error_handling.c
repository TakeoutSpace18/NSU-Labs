#include <stdio.h>

#include "error_handling.h"

void printExitCodeDescription(FILE *stream, exitCode_t exitCode) {
    switch (exitCode)
    {
    case ALLOCATION_ERROR:
        perror("Allocation error");
        break;
    case INPUT_ERROR:
        fprintf(stream, "Wrong input\n");
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
    case NO_SPANNING_TREE:
        fprintf(stream, "no spanning tree\n");
        break;
    case OUT_OF_RANGE:
        fprintf(stream, "index out of range\n");
        break;
    case INVALID_ARGUMENT:
        fprintf(stream, "invalid argument\n");
        break;

    default:
        break;
    }
}
