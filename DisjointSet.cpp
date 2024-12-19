// DisjointSet.cpp
#include "DisjointSet.h"

DisjointSet::DisjointSet(int n) : parent(n), rank(n, 0) 
{
    for(int i = 0; i < n; ++i)
        parent[i] = i;
}

int DisjointSet::find(int u) 
{
    if(parent[u] != u)
        parent[u] = find(parent[u]);
    return parent[u];
}

void DisjointSet::unite(int u, int v) 
{
    // Find the root of the set that contains u
    u = find(u);
    // Find the root of the set that contains v
    v = find(v);
    
    // If u and v are in different sets, perform union
    if (rank[u] < rank[v]) {
        // Attach the tree with root u to the tree with root v
        parent[u] = v;
    } else if (rank[u] > rank[v]) {
        // Attach the tree with root v to the tree with root u
        parent[v] = u;
    } else {
        // If both trees have the same rank, attach one to the other
        // and increment the rank of the resulting tree's root
        parent[v] = u;
        rank[u]++;
    }
}
