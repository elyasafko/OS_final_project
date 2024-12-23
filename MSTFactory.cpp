// MSTFactory.cpp
#include "MSTFactory.h"
#include "PrimAlgorithm.h"
#include "KruskalAlgorithm.h"

MSTAlgorithm *MSTFactory::createAlgorithm(const std::string &name)
{
    if (name == "Prim")
    {
        return new PrimAlgorithm();
    }
    else if (name == "Kruskal")
    {
        return new KruskalAlgorithm();
    }
    else
    {
        return nullptr;
    }
    // in case we would like to add more algorithms in the future, we can add them here.
}
