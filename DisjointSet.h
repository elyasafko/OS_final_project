// DisjointSet.h
#ifndef DISJOINTSET_H
#define DISJOINTSET_H

#include <vector>

class DisjointSet 
{
public:
    DisjointSet(int n);
    int find(int u);
    void unite(int u, int v);
private:
    std::vector<int> parent;
    std::vector<int> rank;
};

#endif // DISJOINTSET_H
