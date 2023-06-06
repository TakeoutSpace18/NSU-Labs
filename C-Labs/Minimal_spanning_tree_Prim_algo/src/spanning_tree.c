#include "spanning_tree.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include "error_handling.h"
#include "priority_queue.h"

static int edgeCompare(const void *a, const void *b) {
    const Edge_t *a_edge = a;
    const Edge_t *b_edge = b;

    if (a_edge->length < b_edge->length) {
        return 1;
    }
    else if (a_edge->length == b_edge->length) {
        return 0;
    }
    else {
        return -1;
    }
}

exitCode_t findSpanningTree(Graph_t *graph, uint64_t *spanningTreeParents) {
    exitCode_t exitCode = SUCCESS;

    bool *verticesInMST = NULL;
    uint32_t *minDistance = NULL;
    PriorityQueue_t *queue = NULL;

    if (!graph || !spanningTreeParents) {
        log_error("null pointer passed");
        return_defer(INVALID_ARGUMENT);
    }

    verticesInMST = calloc(getVertexCount(graph), sizeof(*verticesInMST));
    minDistance = malloc(sizeof(uint32_t) * getVertexCount(graph));
    if (!verticesInMST || !minDistance) {
        return_defer(ALLOCATION_ERROR);
    }

    for (size_t i = 0; i < getVertexCount(graph); ++i) {
        minDistance[i] = UINT32_MAX;
    }

    queue = createPQueue(10, sizeof(Edge_t), &edgeCompare);
    if (!queue) {
        log_error("failed to create priority queue");
        return_defer(ALLOCATION_ERROR);
    }


    uint64_t spanningVertices = 0;
    pqAdd(queue, &(Edge_t){.vertex = 0, .length = 0});

    while (!pqIsEmpty(queue)) {
        Edge_t bestEdge;
        pqPop(queue, &bestEdge);

        if (verticesInMST[bestEdge.vertex]) {
            continue;
        }

        verticesInMST[bestEdge.vertex] = true;

        EdgeIterator_t it = constructEdgeIterator(graph, bestEdge.vertex);
        for (Edge_t e; nextEdge(&it, &e); ) {
            if (!verticesInMST[e.vertex] && e.length < minDistance[e.vertex]) {
                minDistance[e.vertex] = e.length;
                spanningTreeParents[e.vertex] = bestEdge.vertex;
                pqAdd(queue, &(Edge_t){.vertex = e.vertex, .length = e.length});
            }
        }
        spanningVertices++;
    }

    if (spanningVertices < getVertexCount(graph)) {
        return_defer(NO_SPANNING_TREE);
    }

defer:
    free(minDistance);
    free(verticesInMST);
    destroyPQueue(&queue);
    return exitCode;
}
