#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#include "error_handling.h"

typedef struct Edge {
    uint32_t u;
    uint32_t v;
    uint32_t length;
} Edge_t;

typedef struct Graph {
    Edge_t *edges;
    uint32_t vertCount;
    uint32_t edgesCount;
} Graph_t;

Graph_t *createGraph(uint32_t vertexCount, uint32_t maxEdgesCount);
void destroyGraph(Graph_t **graph);

exitCode_t addEdge(Graph_t *graph, uint32_t u, uint32_t v, uint32_t length);

#endif
