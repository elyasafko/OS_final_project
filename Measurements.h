// Measurements.h
#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include <vector>
#include "Edge.h"
#include "Graph.h"

double calculateTotalWeight(const std::vector<Edge>& edges);
Graph buildMSTGraph(int numVertices, const std::vector<Edge>& edges);
std::pair<double, double> calculateDistancesInMST(const Graph& mst);
double calculateAverageDistance(const Graph& graph);

#endif // MEASUREMENTS_H
