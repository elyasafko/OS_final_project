// MSTAlgorithm.h
#ifndef MSTALGORITHM_H
#define MSTALGORITHM_H

#include <vector>
#include <string>
#include "Edge.h"
#include "Graph.h"

class MSTAlgorithm
{
public:
    virtual std::vector<Edge> computeMST(Graph &graph) = 0;
    virtual std::string getComputationLog() const = 0; // Added method
    virtual ~MSTAlgorithm() {}
};

#endif // MSTALGORITHM_H
