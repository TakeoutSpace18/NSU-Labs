#include <stdlib.h>
#include <assert.h>

#include "spanning_tree.h"
#include "error_handling.h"
#include "dsu.h"

static int edgeComapre(const void *a, const void *b) {
    Edge_t *a_edge = (Edge_t*)a;
    Edge_t *b_edge = (Edge_t*)b;
    return a_edge->length > b_edge->length;
}

exitCode_t findSpanningTree(Graph_t *graph, Edge_t *spanningTree) {
    if (!graph || !spanningTree) {
        return INVALID_ARGUMENT;
    }

    uint32_t spanningEdges = 0;
    DSU_t *subsets = createDSU(graph->vertCount);
    if (!subsets) {
        return ALLOCATION_ERROR;
    }

    qsort(graph->edges, graph->edgesCount, sizeof(Edge_t), edgeComapre);

    for (uint32_t i = 0; i < graph->edgesCount; ++i) {
        Edge_t curEdge = graph->edges[i];
        int u_root = find(subsets, curEdge.u);
        int v_root = find(subsets, curEdge.v);

        if (u_root != v_root) {
            unite(subsets, u_root, v_root);
            spanningTree[spanningEdges] = curEdge;
            spanningEdges++;
            if (spanningEdges == graph->vertCount - 1) {
                break;
            }
        }
    }

    destroyDSU(&subsets);

    if (spanningEdges < graph->vertCount - 1) {
        return NO_SPANNING_TREE;
    }
    return SUCCESS;
}
