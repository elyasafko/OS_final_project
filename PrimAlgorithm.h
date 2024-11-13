// PrimAlgorithm.h
#ifndef PRIMALGORITHM_H
#define PRIMALGORITHM_H

#include "MSTAlgorithm.h"

class PrimAlgorithm : public MSTAlgorithm {
public:
    std::vector<Edge> computeMST(Graph& graph) override;
    std::string getComputationLog() const override;  // Implemented method
private:
    std::string computationLog;  // Stores computation steps
};

#endif // PRIMALGORITHM_H
