// Edge.h
#ifndef EDGE_H
#define EDGE_H

struct Edge
{
    int src;
    int dest;
    double weight;

    Edge(int s, int d, double w) : src(s), dest(d), weight(w) {}
};

#endif // EDGE_H
