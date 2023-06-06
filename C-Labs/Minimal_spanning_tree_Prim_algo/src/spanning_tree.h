#ifndef SPANNING_TREE_H
#define SPANNING_TREE_H

#include "graph.h"
#include "error_handling.h"

exitCode_t findSpanningTree(Graph_t *graph, uint64_t *spanningTreeParents);

#endif
