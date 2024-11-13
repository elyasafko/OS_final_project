// KruskalAlgorithm.cpp
#include "KruskalAlgorithm.h"
#include <algorithm>
#include <sstream>

std::vector<Edge> KruskalAlgorithm::computeMST(Graph &graph)
{
    size_t V = graph.getNumVertices();
    std::vector<Edge> allEdges;
    std::vector<Edge> mstEdges;
    DisjointSet ds(V);

    std::stringstream log;
    log << "Starting Kruskal's algorithm:\n";

    // Collect all edges from the adjacency list
    for (size_t u = 0; u < V; ++u)
    {
        for (auto &edge : graph.getAdjEdges(u))
        {
            if (edge.src < edge.dest) // Avoid duplicates in undirected graph
                allEdges.push_back(edge);
        }
    }

    // Sort all edges by weight
    std::sort(allEdges.begin(), allEdges.end(),
              [](Edge &e1, Edge &e2)
              { return e1.weight < e2.weight; });
    log << "Edges sorted by weight:\n";
    for (const auto &edge : allEdges)
    {
        log << "(" << edge.src << ", " << edge.dest << ") - Weight: " << edge.weight << "\n";
    }

    // Kruskal's algorithm
    for (auto &edge : allEdges)
    {
        int uSet = ds.find(edge.src);
        int vSet = ds.find(edge.dest);

        if (uSet != vSet)
        {
            mstEdges.push_back(edge);
            ds.unite(uSet, vSet);
            log << "Include edge (" << edge.src << ", " << edge.dest << ") with weight " << edge.weight << " in MST.\n";
        }
        else
        {
            log << "Skipping edge (" << edge.src << ", " << edge.dest << ") to avoid cycle.\n";
        }

        if (mstEdges.size() == V - 1)
            break;
    }

    // Store the log for later retrieval
    computationLog = log.str();

    return mstEdges;
}

std::string KruskalAlgorithm::getComputationLog() const
{
    return computationLog;
}
