// KruskalAlgorithm.h
#ifndef KRUSKALALGORITHM_H
#define KRUSKALALGORITHM_H

#include "MSTAlgorithm.h"
#include "DisjointSet.h"

class KruskalAlgorithm : public MSTAlgorithm {
public:
    std::vector<Edge> computeMST(Graph& graph) override;
    std::string getComputationLog() const override;  // Implemented method
private:
    std::string computationLog;  // Stores computation steps
};

#endif // KRUSKALALGORITHM_H
