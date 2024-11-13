// MSTFactory.h
#ifndef MSTFACTORY_H
#define MSTFACTORY_H

#include <string>
#include "MSTAlgorithm.h"

class MSTFactory {
public:
    static MSTAlgorithm* createAlgorithm(const std::string& name);
};

#endif // MSTFACTORY_H
