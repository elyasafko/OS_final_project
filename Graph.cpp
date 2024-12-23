// Graph.cpp
#include "Graph.h"
#include <algorithm>

Graph::Graph(int vertices) : V(vertices), adjList(vertices) {}

void Graph::addEdge(int src, int dest, double weight)
{
    Edge edge1(src, dest, weight);
    Edge edge2(dest, src, weight);
    adjList[src].push_back(edge1);
    adjList[dest].push_back(edge2);
}

void Graph::removeEdge(int src, int dest)
{
    adjList[src].erase(std::remove_if(adjList[src].begin(), adjList[src].end(),
                                      [dest](Edge &e)
                                      { return e.dest == dest; }),
                       adjList[src].end());

    adjList[dest].erase(std::remove_if(adjList[dest].begin(), adjList[dest].end(),
                                       [src](Edge &e)
                                       { return e.dest == src; }),
                        adjList[dest].end());
}

int Graph::getNumVertices() const
{
    return V;
}

const std::vector<Edge> &Graph::getAdjEdges(int vertex) const
{
    return adjList[vertex];
}
