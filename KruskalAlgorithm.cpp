// KruskalAlgorithm.cpp
#include "KruskalAlgorithm.h"
#include <algorithm>
#include <sstream>

/**
 * @brief Computes the Minimum Spanning Tree (MST) of a graph using Kruskal's algorithm.
 *
 * This function implements Kruskal's algorithm to find the MST of the given graph.
 * It first collects all edges in the graph and sorts them by weight.
 * Then it repeatedly selects the edge with the minimum weight that does not form
 * a cycle in the MST, until the MST spans all vertices.
 *
 * The function also maintains a log of the computation steps, which can be retrieved
 * using getComputationLog().
 * 
 * we use disjoint set data structure.
 * its suits well for this algorithm because its head is always the lowest node in the tree.
 *
 * @param graph The input graph represented as an adjacency list.
 *
 * @return A vector of edges representing the MST of the input graph.
 */
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
