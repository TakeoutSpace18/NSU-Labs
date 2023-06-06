#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#include "Graph.h"

Graph* CreateGraph(int vertexCount) {
    assert(vertexCount > 0);
    Graph* graph = malloc(sizeof(Graph));
    graph->vertCount = vertexCount;

    // Create bit matrix
    const int bitsPerEntry = sizeof(**graph->edges) * CHAR_BIT;
    int stringLen = (vertexCount + bitsPerEntry - 1) / bitsPerEntry;

    graph->edges = calloc(vertexCount, sizeof(*graph->edges));
    for (int i = 0; i < vertexCount; ++i) {
        graph->edges[i] = calloc(stringLen, sizeof(**graph->edges));
    }
    return graph;
}

void DestroyGraph(Graph* graph) {
    assert(graph != NULL);
    for (int i = 0; i < GetVertexCount(graph); ++i) {
        free(graph->edges[i]);
    }
    free(graph->edges);
    free(graph);
}

void AddEdge(Graph* graph, int u, int v) {
    assert(0 <= u && u < graph->vertCount);
    assert(0 <= v && v < graph->vertCount);

    const int bitsPerEntry = sizeof(**graph->edges) * CHAR_BIT;
    graph->edges[u][v / bitsPerEntry] |= (0x1 << (v % bitsPerEntry));
}

bool HasEdge(const Graph* graph, int u, int v) {
    assert(0 <= u && u < graph->vertCount);
    assert(0 <= v && v < graph->vertCount);

    const int bitsPerEntry = sizeof(**graph->edges) * CHAR_BIT;
    return graph->edges[u][v / bitsPerEntry] & (0x1 << (v % bitsPerEntry));
}

int GetVertexCount(const Graph* graph) {
    return graph->vertCount;
}

EdgeIterator ConstructEdgeIterator(const Graph* graph, int nodeIdx) {
    EdgeIterator it;
    it.graph = graph;
    it.nodeIdx = nodeIdx;
    it.curChildIdx = -1;
    return it;
}

int NextEdge(EdgeIterator* it) {
    for (int i = it->curChildIdx + 1; i < it->graph->vertCount; ++i) {
        if (HasEdge(it->graph, it->nodeIdx, i)) {
            it->curChildIdx = i;
            return i;
        }
    }
    it->curChildIdx = -1;
    return -1;
}
