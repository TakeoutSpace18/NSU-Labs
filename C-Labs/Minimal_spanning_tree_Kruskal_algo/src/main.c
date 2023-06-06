#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#include "graph.h"
#include "error_handling.h"
#include "spanning_tree.h"

#define return_defer(value) do { \
    exitCode = (value); \
    goto defer; \
} while (0)

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

    FILE *input = fopen("in.txt", "r");
    FILE *output = fopen("out.txt", "w");
    Graph_t *graph = NULL;
    Edge_t *spanningTree = NULL;

    int vertN, edgesN;
    if (fscanf(input, "%i%i", &vertN, &edgesN) != 2) {
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
    graph = createGraph(vertN, edgesN);
    if (!graph) {
        return_defer(ALLOCATION_ERROR);
    }

    for (int i = 0; i < edgesN; ++i) {
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

    spanningTree = malloc(sizeof(Edge_t) * (vertN - 1));
    if (!spanningTree) {
        return_defer(ALLOCATION_ERROR);
    }

    exitCode = findSpanningTree(graph, spanningTree);
    if (exitCode != SUCCESS) {
        return_defer(exitCode);
    }

    // Print answer
    for (int i = 0; i < vertN - 1; ++i) {
        fprintf(output, "%u %u\n", spanningTree[i].u + 1, spanningTree[i].v + 1);
    }

defer:
    printExitCodeDescription(output, exitCode);
    fclose(input);
    fclose(output);
    free(spanningTree);
    destroyGraph(&graph);
    // return exitCode;
    return SUCCESS; // for testing system
}
