// Graph.h
#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include "Edge.h"

class Graph
{
public:
    Graph(int vertices);
    void addEdge(int src, int dest, double weight);
    void removeEdge(int src, int dest);
    int getNumVertices() const;
    const std::vector<Edge> &getAdjEdges(int vertex) const;

private:
    int V;
    std::vector<std::vector<Edge>> adjList;
};

#endif // GRAPH_H
