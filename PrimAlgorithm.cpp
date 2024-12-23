// PrimAlgorithm.cpp
#include "PrimAlgorithm.h"
#include <queue>
#include <functional>
#include <sstream>

/**
 * @brief Computes the Minimum Spanning Tree (MST) of a graph using Prim's algorithm.
 * 
 * This function implements Prim's algorithm to find the MST of the given graph.
 * It starts from vertex 0 and proceeds to add the smallest edge connecting a 
 * vertex inside the MST to a vertex outside the MST until the MST spans all 
 * vertices. The function maintains a priority queue to efficiently select the 
 * next edge with the minimum weight.
 * 
 * @param graph The input graph represented as an adjacency list.
 * 
 * @return A vector of edges representing the MST of the input graph.
 */

std::vector<Edge> PrimAlgorithm::computeMST(Graph &graph)
{
    size_t V = graph.getNumVertices();
    // Keep track of which vertices are already included in the MST
    std::vector<bool> inMST(V, false);
    std::vector<Edge> mstEdges;

    // Create a min-heap (priority queue) to efficiently select the next
    // edge with the minimum weight. The heap is ordered by the edge weights.
    auto comp = [](Edge &e1, Edge &e2)
    { return e1.weight > e2.weight; };
    std::priority_queue<Edge, std::vector<Edge>, decltype(comp)> pq(comp);

    // Create a log to store computation steps
    std::stringstream log;
    log << "Starting Prim's algorithm:\n";

    // Start from vertex 0, which is always included in the MST
    inMST[0] = true;
    log << "Include vertex 0 in MST.\n";
    // Add all adjacent edges of vertex 0 to the priority queue
    for (auto &edge : graph.getAdjEdges(0))
    {
        pq.push(edge);
        log << "Add edge (" << edge.src << ", " << edge.dest << ") with weight " << edge.weight << " to the priority queue.\n";
    }

    // While the priority queue is not empty and we still need to select
    // more edges to complete the MST
    while (!pq.empty() && mstEdges.size() < V - 1)
    {
        // Select the edge with the minimum weight from the priority queue
        Edge edge = pq.top();
        pq.pop();

        // v is the destination vertex of the selected edge
        int v = edge.dest;
        // If v is not yet included in the MST
        if (!inMST[v])
        {
            // Include v in the MST
            inMST[v] = true;
            // Add the selected edge to the MST
            mstEdges.push_back(edge);
            log << "Include edge (" << edge.src << ", " << edge.dest << ") with weight " << edge.weight << " in MST.\n";

            // Add all adjacent edges of v to the priority queue
            for (auto &adjEdge : graph.getAdjEdges(v))
            {
                // If the adjacent edge is not yet included in the MST
                if (!inMST[adjEdge.dest])
                {
                    // Add it to the priority queue
                    pq.push(adjEdge);
                    log << "Add edge (" << adjEdge.src << ", " << adjEdge.dest << ") with weight " << adjEdge.weight << " to the priority queue.\n";
                }
            }
        }
    }

    // Store the log for later retrieval
    computationLog = log.str();

    return mstEdges;
}

std::string PrimAlgorithm::getComputationLog() const
{
    return computationLog;
}
