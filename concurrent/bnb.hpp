#include "path.hpp"
#include "matrix.hpp"
#include <iostream>

#ifndef BNB_HPP
#define BNB_HPP

// Branch and bound algorithm
class BnB {
private:
    EdgeMatrix _left;
    EdgeMatrix _right;

    void get_child_path(EdgeMatrix edges) {
        int order = edges.size();
        bool found = false;
        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (i != j && edges[i][j] == 0) {
                    found = true;
                    _left[i][j] = 1;
                    _left[j][i] = 1;
                    _left = update_edges(_left, i);
                    _right[i][j] = -1;
                    _right[j][i] = -1;
                    _right = update_edges(_right, i);
                    break;
                }
            }
            if (found) break;
        }

        if (!found) throw 1;
    }

public:
    explicit BnB(EdgeMatrix edges) {
        _left = edges;
        _right = edges;
        get_child_path(edges);
    }

    /**
     * Update the edges of the path.
     * If the vertex already as 2 edges, then every other edge is set to -1.
     * If the vertex has 1 edge and there is only one edge left, then the
     * remaining edge is set to 1.
     * If the vertex has 0 edges, and there is only two edges left, then the
     * remaining edges are set to 1.
     * @param edges The edges to update.
     * @param vertex The vertex to update.
    */
    EdgeMatrix update_edges(EdgeMatrix edges, int vertex) {
        int order = edges.size();

        int edgesUsed = 0;
        int edgesLeft = 0;

        for (int i = 0; i < order; i++) {
            if (edges[vertex][i] == 1) {
                edgesUsed++;
            } else if (edges[vertex][i] == 0 && i != vertex) {
                edgesLeft++;
            }
        }

        if (edgesUsed == 2) {
            for (int i = 0; i < order; i++) {
                if (edges[vertex][i] == 0 && i != vertex) {
                    edges[vertex][i] = -1;
                    edges[i][vertex] = -1;
                }
            }
        } else if (edgesUsed == 1 && edgesLeft == 1) {
            for (int i = 0; i < order; i++) {
                if (edges[vertex][i] == 0 && i != vertex) {
                    edges[vertex][i] = 1;
                    edges[i][vertex] = 1;
                }
            }
        } else if (edgesUsed == 0 && edgesLeft == 2) {
            for (int i = 0; i < order; i++) {
                if (edges[vertex][i] == 0) {
                    edges[vertex][i] = 1;
                    edges[i][vertex] = 1;
                }
            }
        }

        return edges;
    }

    EdgeMatrix left() {
        return _left;
    }

    EdgeMatrix right() {
        return _right;
    }
};

#endif // BNB_HPP