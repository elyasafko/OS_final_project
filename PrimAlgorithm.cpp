// PrimAlgorithm.cpp
#include "PrimAlgorithm.h"
#include <queue>
#include <functional>
#include <sstream>

std::vector<Edge> PrimAlgorithm::computeMST(Graph &graph)
{
    size_t V = graph.getNumVertices();
    std::vector<bool> inMST(V, false);
    std::vector<Edge> mstEdges;
    auto comp = [](Edge &e1, Edge &e2)
    { return e1.weight > e2.weight; };
    std::priority_queue<Edge, std::vector<Edge>, decltype(comp)> pq(comp);

    std::stringstream log;
    log << "Starting Prim's algorithm:\n";

    // Start from vertex 0
    inMST[0] = true;
    log << "Include vertex 0 in MST.\n";
    for (auto &edge : graph.getAdjEdges(0))
    {
        pq.push(edge);
        log << "Add edge (" << edge.src << ", " << edge.dest << ") with weight " << edge.weight << " to the priority queue.\n";
    }

    while (!pq.empty() && mstEdges.size() < V - 1)
    {
        Edge edge = pq.top();
        pq.pop();

        int v = edge.dest;
        if (!inMST[v])
        {
            inMST[v] = true;
            mstEdges.push_back(edge);
            log << "Include edge (" << edge.src << ", " << edge.dest << ") with weight " << edge.weight << " in MST.\n";

            for (auto &adjEdge : graph.getAdjEdges(v))
            {
                if (!inMST[adjEdge.dest])
                {
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
