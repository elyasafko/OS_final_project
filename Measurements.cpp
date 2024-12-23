#include "Measurements.h"
#include <queue>
#include <limits>
#include <iostream>
#include <vector>
#include <functional>

// Helper function: Run Dijkstra to get distances from 'start' to all other vertices
static std::vector<double> dijkstraDistances(const Graph &graph, int start)
{
    int n = graph.getNumVertices();
    std::vector<double> dist(n, std::numeric_limits<double>::infinity());
    dist[start] = 0.0;

    // Min-heap of (distance, node)
    using NodeDistPair = std::pair<double, int>;
    std::priority_queue<NodeDistPair, std::vector<NodeDistPair>, std::greater<NodeDistPair>> pq;
    pq.push({0.0, start});

    while (!pq.empty())
    {
        auto [currentDist, node] = pq.top();
        pq.pop();

        // If we already found a better path, skip
        if (currentDist > dist[node])
            continue;

        // Explore neighbors
        for (const auto &edge : graph.getAdjEdges(node))
        {
            double newDist = currentDist + edge.weight;
            if (newDist < dist[edge.dest])
            {
                dist[edge.dest] = newDist;
                pq.push({newDist, edge.dest});
            }
        }
    }

    return dist;
}

// ---------------------------------------------------
// 1) Calculate total weight
double calculateTotalWeight(const std::vector<Edge> &edges)
{
    double total = 0.0;
    for (const auto &edge : edges)
    {
        total += edge.weight;
    }
    return total;
}

// 2) Build MST graph from MST edges
Graph buildMSTGraph(int numVertices, const std::vector<Edge> &edges)
{
    Graph mst(numVertices);
    for (const auto &edge : edges)
    {
        mst.addEdge(edge.src, edge.dest, edge.weight);
    }
    return mst;
}

// 3) Calculate distances in MST (longest and shortest among all distinct pairs)
std::pair<double, double> calculateDistancesInMST(const Graph &mst)
{
    int n = mst.getNumVertices();
    if (n <= 1)
    {
        // If there's 0 or 1 vertex, no meaningful pair distances
        return {0.0, 0.0};
    }

    double maxDist = 0.0;
    double minDist = std::numeric_limits<double>::infinity();

    // For each vertex, run Dijkstra in the MST
    for (int i = 0; i < n; i++)
    {
        std::vector<double> dist = dijkstraDistances(mst, i);
        for (int j = 0; j < n; j++)
        {
            if (i == j) 
                continue; // skip distance(i,i)=0

            if (dist[j] < std::numeric_limits<double>::infinity())
            {
                if (dist[j] > maxDist)
                    maxDist = dist[j];
                if (dist[j] < minDist)
                    minDist = dist[j];
            }
        }
    }

    // If we never updated minDist, it means the MST might be disconnected (shouldn't happen for a proper MST)
    if (minDist == std::numeric_limits<double>::infinity())
        minDist = 0.0;

    return {maxDist, minDist};
}

// 4) Calculate average distance over all pairs in a general graph
double calculateAverageDistance(const Graph &graph)
{
    int n = graph.getNumVertices();
    if (n <= 1) 
        return 0.0; // 0 or 1 vertex => no meaningful pairs

    double sumOfDistances = 0.0;
    int count = 0;

    // For each vertex, run Dijkstra
    for (int i = 0; i < n; i++)
    {
        std::vector<double> dist = dijkstraDistances(graph, i);

        // Sum distances to j (where j > i) to avoid double counting
        for (int j = i + 1; j < n; j++)
        {
            if (dist[j] < std::numeric_limits<double>::infinity())
            {
                sumOfDistances += dist[j];
                count++;
            }
        }
    }

    // If count == 0, graph might be completely disconnected
    if (count == 0)
        return 0.0;

    double average = sumOfDistances / static_cast<double>(count);
    return average;
}
