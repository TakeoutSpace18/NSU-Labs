#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "graph.h"
#include "error_handling.h"
#include "shortest_path.h"

bool validateVerticesNumber(int vertN) {
    return (0 <= vertN && vertN <= 5000);
}

bool validateEdgesNumber(int vertN, int edgesN) {
    int maxEdgesN = vertN * (vertN - 1) / 2;
    return (0 <= edgesN && edgesN <= maxEdgesN);
}

bool validateVertexIndex(int vertN, int idx) {
    return (1 <= idx && idx <= vertN);
}

bool validateVertexLength(int len) {
    return (0 <= len && len <= INT_MAX);
}

int main(void) {
    exitCode_t exitCode = SUCCESS;

    Graph_t *graph = NULL;
    ShortestPathData_t *ans = NULL;
    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    size_t vertN, start, finish, edgesN;
    if (fscanf(input, "%zu%zu%zu%zu", &vertN, &start, &finish, &edgesN) != 4) {
        return_defer(BAD_NUMBER_OF_LINES);
    }
    if (!validateVerticesNumber(vertN)) {
        return_defer(BAD_NUMBER_OF_VERTICES);
    }
    if (!validateVertexIndex(vertN, start) || !validateVertexIndex(vertN, finish)) {
        return_defer(BAD_VERTEX);
    }
    if (!validateEdgesNumber(vertN, edgesN)) {
        return_defer(BAD_NUMBER_OF_EDGES);
    }

    // Read graph
    graph = createGraph(vertN);
    if (!graph) {
        return_defer(ALLOCATION_ERROR);
    }

    for (uint64_t i = 0; i < edgesN; ++i) {
        int u, v, len;
        if (fscanf(input, "%i%i%i", &u, &v, &len) != 3) {
            return_defer(BAD_NUMBER_OF_LINES);
        }
        if (!validateVertexIndex(vertN, u)) {
            return_defer(BAD_VERTEX);
        }
        if (!validateVertexIndex(vertN, v)) {
            return_defer(BAD_VERTEX);
        }
        if (!validateVertexLength(len)) {
            return_defer(BAD_LENGTH);
        }

        // Make numeration start from zero
        u--; v--;
        exitCode = addEdge(graph, u, v, len);
        if (exitCode != SUCCESS) {
            return_defer(exitCode);
        }
    }

    // Make numeration start from zero
    start--; finish--;

    ans = createShortestPathData(vertN);
    exitCode = findShortestPath(graph, start, finish, ans);

    if (!(exitCode == SUCCESS || exitCode == OVERFLOW || exitCode == NO_PATH)) {
        return_defer(exitCode);
    }

    // Print distance
    for (size_t i = 0; i < vertN; ++i) {
        if (ans->distances[i] == UINT64_MAX) {
            fprintf(output, "oo ");
        }
        else if (ans->distances[i] > INT_MAX) {
            fprintf(output, "INT_MAX+ ");
        }
        else {
            fprintf(output, "%lu ", ans->distances[i]);
        }
    }

    fprintf(output, "\n");

    // Print path
    if (exitCode == NO_PATH) {
        fprintf(output, "no path\n");
    }
    else if (exitCode == OVERFLOW) {
        fprintf(output, "overflow\n");
    }
    else {
        for (size_t i = ans->pathLen - 1; i != (size_t) -1; --i) {
            fprintf(output, "%lu ", ans->path[i] + 1);
        }
    }

defer:
    printExitCodeDescription(output, exitCode);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }
    destroyGraph(&graph);
    destroyShortestPathData(&ans);
    // return exitCode;
    return SUCCESS; // for testing system
}
