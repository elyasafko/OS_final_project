// DisjointSet.cpp
#include "DisjointSet.h"

DisjointSet::DisjointSet(int n) : parent(n), rank(n, 0) {
    for(int i = 0; i < n; ++i)
        parent[i] = i;
}

int DisjointSet::find(int u) {
    if(parent[u] != u)
        parent[u] = find(parent[u]);
    return parent[u];
}

void DisjointSet::unite(int u, int v) {
    u = find(u);
    v = find(v);
    if(rank[u] < rank[v]) {
        parent[u] = v;
    } else if(rank[u] > rank[v]) {
        parent[v] = u;
    } else {
        parent[v] = u;
        rank[u]++;
    }
}
