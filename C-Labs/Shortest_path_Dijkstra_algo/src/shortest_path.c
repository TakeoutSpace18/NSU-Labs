#include "shortest_path.h"

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

#include "error_handling.h"
#include "priority_queue.h"

ShortestPathData_t *createShortestPathData(size_t vertN) {
    ShortestPathData_t *data = malloc(sizeof(ShortestPathData_t));
    if (!data) {
        return NULL;
    }

    data->distances = malloc(sizeof(uint64_t) * vertN);
    data->path = malloc(sizeof(uint64_t) * vertN);
    if (!data->distances || !data->path) {
        free(data->distances);
        free(data->path);
        free(data);
        return NULL;
    }
    return data;
}

void destroyShortestPathData(ShortestPathData_t **data) {
    if (!data || !(*data)) {
        return;
    }
    free((*data)->distances);
    free((*data)->path);
    free(*data);
    *data = NULL;
}

typedef struct DijkstraNode {
    uint64_t vertex;
    uint64_t dist;
} DijkstraNode_t;

static int nodeCompare(const void *a, const void *b) {
    const DijkstraNode_t *a_node = a;
    const DijkstraNode_t *b_node = b;

    if (a_node->dist < b_node->dist) {
        return 1;
    }
    else if (a_node->dist == b_node->dist) {
        return 0;
    }
    else {
        return -1;
    }
}

static size_t convertToPath(uint64_t vertex, uint64_t *parents, ShortestPathData_t *data) {
    if (vertex == UINT64_MAX) {
        return 0;
    }
    size_t idx = convertToPath(parents[vertex], parents, data);
    data->path[idx] = vertex;
    return idx + 1;
}


exitCode_t findShortestPath(Graph_t *graph, uint64_t start, uint64_t finish, ShortestPathData_t *data) {
    exitCode_t exitCode = SUCCESS;

    bool *visited = NULL;
    uint64_t *parents = NULL;
    PriorityQueue_t *queue = NULL;

    if (!graph) {
        log_error("null pointer passed");
        return_defer(INVALID_ARGUMENT);
    }

    visited = calloc(getVertexCount(graph), sizeof(bool));
    parents = malloc(sizeof(uint64_t) * getVertexCount(graph));
    if (!visited || !parents) {
        log_error("allocation failed");
        return_defer(ALLOCATION_ERROR);
    }

    for (size_t i = 0; i < getVertexCount(graph); ++i) {
        data->distances[i] = UINT64_MAX;
        parents[i] = UINT64_MAX;
    }
    data->distances[start] = 0;

    queue = createPQueue(getVertexCount(graph), sizeof(DijkstraNode_t), &nodeCompare);
    if (!queue) {
        log_error("failed to create priority queue");
        return_defer(ALLOCATION_ERROR);
    }

    int overflowCount = 0;
    pqAdd(queue, &(DijkstraNode_t){.vertex = start, .dist = 0});

    while (!pqIsEmpty(queue)) {
        DijkstraNode_t curNode;
        pqPop(queue, &curNode);

        if (visited[curNode.vertex]) {
            continue;
        }

        visited[curNode.vertex] = true;

        EdgeIterator_t it = constructEdgeIterator(graph, curNode.vertex);
        for (Edge_t e; nextEdge(&it, &e); ) {
            uint64_t newLength = curNode.dist + e.length;
            if (!visited[e.vertex] && newLength < data->distances[e.vertex]) {
                if (newLength >= INT_MAX) {
                    overflowCount++;
                }
                parents[e.vertex] = curNode.vertex;
                data->distances[e.vertex] = newLength;
                pqAdd(queue, &(DijkstraNode_t){.vertex = e.vertex, .dist = newLength});
            }
        }
    }

    if (data->distances[finish] == UINT64_MAX) {
        return_defer(NO_PATH);
    }
    if (overflowCount > 2 && data->distances[finish] > INT_MAX) {
        return_defer(OVERFLOW);
    }

    data->pathLen = convertToPath(finish, parents, data);

defer:
    free(visited);
    free(parents);
    destroyPQueue(&queue);
    return exitCode;
}
