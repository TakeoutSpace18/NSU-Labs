#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "graph.h"
#include "error_handling.h"
#include "spanning_tree.h"

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
    uint64_t *spanningTreeParents = NULL;
    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    if (!input || !output) {
        return_defer(IO_ERROR);
    }

    size_t vertN, edgesN;
    if (fscanf(input, "%zu%zu", &vertN, &edgesN) != 2) {
        return_defer(BAD_NUMBER_OF_LINES);
    }
    if (!validateVerticesNumber(vertN)) {
        return_defer(BAD_NUMBER_OF_VERTICES);
    }
    if (!validateEdgesNumber(vertN, edgesN)) {
        return_defer(BAD_NUMBER_OF_EDGES);
    }
    if (vertN == 0) {
        return_defer(NO_SPANNING_TREE);
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

    spanningTreeParents = malloc(sizeof(uint64_t) * vertN);
    if (!spanningTreeParents) {
        return_defer(ALLOCATION_ERROR);
    }

    exitCode = findSpanningTree(graph, spanningTreeParents);
    if (exitCode != SUCCESS) {
        return_defer(exitCode);
    }

    // Print answer
    for (uint64_t i = 1; i < vertN; ++i) {
        fprintf(output, "%lu %lu\n", spanningTreeParents[i] + 1, i + 1);
    }

defer:
    printExitCodeDescription(output, exitCode);
    if (input) {
        fclose(input);
    }
    if (output) {
        fclose(output);
    }
    free(spanningTreeParents);
    destroyGraph(&graph);
    // return exitCode;
    return SUCCESS; // for testing system
}
