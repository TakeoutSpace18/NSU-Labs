#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include <stdbool.h>

#include "error_handling.h"

typedef struct Graph {
    int32_t **adjMatrix;
    size_t vertCount;
} Graph_t;

typedef struct Edge {
    uint64_t vertex;
    uint32_t length;
} Edge_t;

Graph_t *createGraph(uint64_t vertexCount);
void destroyGraph(Graph_t **graph);

exitCode_t addEdge(Graph_t *graph, uint64_t u, uint64_t v, uint64_t length);
bool hasEdge(const Graph_t *graph, uint64_t u, uint64_t v);

size_t getVertexCount(const Graph_t *graph);

typedef struct EdgeIterator {
    const Graph_t* graph;
    int64_t u;
    int64_t curV;
} EdgeIterator_t;

EdgeIterator_t constructEdgeIterator(const Graph_t *graph, uint64_t vertex);
bool nextEdge(EdgeIterator_t *it, Edge_t *dst);

#endif
