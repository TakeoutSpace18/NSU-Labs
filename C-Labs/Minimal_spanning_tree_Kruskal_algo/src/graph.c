#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "graph.h"
#include "error_handling.h"

Graph_t *createGraph(uint32_t vertexCount, uint32_t maxEdgesCount) {
    assert(vertexCount > 0);

    Graph_t* graph = malloc(sizeof(Graph_t));
    if (!graph) {
        return NULL;
    }

    graph->edges = malloc(sizeof(Edge_t) * maxEdgesCount);
    if (!graph->edges) {
        free(graph);
        return NULL;
    }

    graph->vertCount = vertexCount;
    graph->edgesCount = 0;
    return graph;
}

void destroyGraph(Graph_t **graph) {
    if (!graph || !(*graph)) {
        return;
    }
    free((*graph)->edges);
    free(*graph);
    *graph = NULL;
}

exitCode_t addEdge(Graph_t* graph, uint32_t u, uint32_t v, uint32_t length) {
    if (u >= graph->vertCount || v >= graph->vertCount) {
        return OUT_OF_RANGE;
    }

    graph->edges[graph->edgesCount] = (Edge_t){
        .u = u,
        .v = v,
        .length = length
    };

    ++graph->edgesCount;
    return SUCCESS;
}
