#ifndef SHORTEST_PATH_H
#define SHORTEST_PATH_H

#include "graph.h"
#include "error_handling.h"

typedef struct ShortestPathData {
    uint64_t *distances;
    uint64_t *path;
    size_t pathLen;
} ShortestPathData_t;

ShortestPathData_t *createShortestPathData(size_t vertN);
exitCode_t findShortestPath(Graph_t *graph, uint64_t start, uint64_t finish, ShortestPathData_t *data);
void destroyShortestPathData(ShortestPathData_t **data);

#endif
