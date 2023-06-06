#pragma once

typedef struct Graph {
    int vertCount;
    int** edges;
} Graph;

Graph* CreateGraph(int vertexCount);
void DestroyGraph(Graph* graph);

void AddEdge(Graph* graph, int u, int v);
bool HasEdge(const Graph* graph, int u, int v);
int GetVertexCount(const Graph* graph);

typedef struct EdgeIterator {
    const Graph* graph;
    int nodeIdx;
    int curChildIdx;
} EdgeIterator;

EdgeIterator ConstructEdgeIterator(const Graph* graph, int nodeIdx);
int NextEdge(EdgeIterator* it);
