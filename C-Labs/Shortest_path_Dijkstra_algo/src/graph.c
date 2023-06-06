#include "graph.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include <assert.h>

#include "error_handling.h"

static int32_t **allocateAdjMatrix(size_t vertCount) {
    int32_t **matrix = malloc(sizeof(int32_t*) * vertCount);
    if (!matrix) {
        return NULL;
    }

    for (size_t u = 0; u < vertCount; ++u) {
        matrix[u] = malloc(sizeof(**matrix) * vertCount);
        if (!matrix[u]) {
            for (size_t j = 0; j < u; ++j) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }

        for (size_t v = 0; v < vertCount; ++v) {
            // -1 means no edge between vertices
            matrix[u][v] = -1;
        }
    }
    return matrix;
}

Graph_t *createGraph(size_t vertexCount) {
    if (vertexCount == 0) {
        log_error("can't create graph with 0 vertices");
        return NULL;
    }

    Graph_t* graph = malloc(sizeof(Graph_t));
    if (!graph) {
        return NULL;
    }

    graph->adjMatrix = allocateAdjMatrix(vertexCount);
    if (!graph->adjMatrix) {
        log_error("failed to allocate adjacency matrix");
        free(graph);
        return NULL;
    }

    graph->vertCount = vertexCount;
    return graph;
}

void destroyGraph(Graph_t **graph) {
    if (!graph || !(*graph)) {
        return;
    }
    for (size_t i = 0; i < (*graph)->vertCount; ++i) {
        free((*graph)->adjMatrix[i]);
    }
    free((*graph)->adjMatrix);
    free(*graph);
    *graph = NULL;
}

exitCode_t addEdge(Graph_t* graph, uint64_t u, uint64_t v, uint32_t length) {
    if (u >= graph->vertCount || v >= graph->vertCount) {
        log_error("vertex index out of range");
        return OUT_OF_BOUNDS;
    }

    graph->adjMatrix[u][v] = length;
    graph->adjMatrix[v][u] = length;

    return SUCCESS;
}

bool hasEdge(const Graph_t *graph, uint64_t u, uint64_t v) {
    if (!graph) {
        return false;
    }
    return graph->adjMatrix[u][v] != -1;
}

size_t getVertexCount(const Graph_t *graph) {
    return graph->vertCount;
}

EdgeIterator_t constructEdgeIterator(const Graph_t *graph, uint64_t vertex) {
    assert(graph);
    EdgeIterator_t it;
    it.graph = graph;
    it.u = vertex;
    it.curV = -1;
    return it;
}

bool nextEdge(EdgeIterator_t *it, Edge_t *dst) {
    if (!it || !dst) {
        return false;
    }

    for (uint64_t v = it->curV + 1; v < it->graph->vertCount; ++v) {
        if (hasEdge(it->graph, it->u, v)) {
            dst->length = it->graph->adjMatrix[it->u][v];
            dst->vertex = v;

            it->curV = v;
            return true;
        }
    }
    it->curV = -1;
    return false;
}
