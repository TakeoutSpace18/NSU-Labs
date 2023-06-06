#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "Graph.h"

void ExitWithMessage(const char* msg) {
    printf("%s\n", msg);
    exit(0);
}

void ValidateVerticesNumber(int vertN) {
    if (vertN < 0 || 2000 < vertN) {
        ExitWithMessage("bad number of vertices");
    }
}

void ValidateEdgesNumber(int vertN, int edgesN) {
    int maxEdgesN = vertN * (vertN - 1) / 2;
    if (edgesN < 0 || maxEdgesN < edgesN) {
        ExitWithMessage("bad number of edges");
    }
}

void ValidateVertexIndex(int vertN, int idx) {
    if (idx < 1 || vertN < idx) {
        ExitWithMessage("bad vertex");
    }
}

typedef struct TopSortData {
    int* sortedResult;
    char* color;
    int count;
} TopSortData;

// Possible vertex colors
enum {
    WHITE = 0, // not visited
    GRAY = 1, // found, currently in dfs
    BLACK = 2 // assigned a number
};

bool TopSortImpl(Graph* graph, int u, TopSortData* data);

bool TopSort(Graph* graph, int* result) {
    TopSortData data;
    data.color = calloc(GetVertexCount(graph), sizeof(char));
    data.count = GetVertexCount(graph) - 1;
    data.sortedResult = result;

    for (int u = 0; u < GetVertexCount(graph); ++u) {
        if (data.color[u] == WHITE) {
            bool success = TopSortImpl(graph, u, &data);
            if (!success)  {
                free(data.color);
                return false; // graph has cycle and sorting is impossible
            }
        }
    }
    free(data.color);
    return true;
}

bool TopSortImpl(Graph* graph, int u, TopSortData* data) {
    if (data->color[u] == GRAY) {
        return false; // has cycle
    }
    if (data->color[u] == BLACK) {
        return true;
    }
    data->color[u] = GRAY;

    EdgeIterator it = ConstructEdgeIterator(graph, u);
    for (int v = NextEdge(&it); v >= 0; v = NextEdge(&it)) {
        bool success = TopSortImpl(graph, v, data);
        if (!success) {
            return false;
        }
    }

    data->sortedResult[data->count--] = u;
    data->color[u] = BLACK;
    return true;
}

int main(void) {
    int vertN, edgesN;
    if (scanf("%i%i", &vertN, &edgesN) == 2) {
        ValidateVerticesNumber(vertN);
        ValidateEdgesNumber(vertN, edgesN);

        // Read graph
        Graph* graph = CreateGraph(vertN);
        for (int i = 0; i < edgesN; ++i) {
            int u, v;
            if (scanf("%i%i", &u, &v) == 2) {
                ValidateVertexIndex(vertN, u);
                ValidateVertexIndex(vertN, v);
                // make numeration start from zero
                u--; v--;
                AddEdge(graph, u, v);
            }
            else  {
                DestroyGraph(graph);
                ExitWithMessage("bad number of lines");
            }
        }

        // Do topological sort
        int* sortResult = malloc(sizeof(int) * vertN);
        bool success = TopSort(graph, sortResult);

        // Print the results
        if (success) {
            for (int i = 0; i < vertN; ++i) {
                printf("%i ", sortResult[i] + 1);
            }
        }
        else {
            printf("impossible to sort\n");
        }
        free(sortResult);
        DestroyGraph(graph);
    }
    else ExitWithMessage("bad number of lines");

    return EXIT_SUCCESS;
}
