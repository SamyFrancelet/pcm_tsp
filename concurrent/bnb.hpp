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

    EdgeMatrix update_edges(EdgeMatrix edges, int vertex) {
        int order = edges.size();

        for (int i = vertex; i < order; i++) {
            int remaining = 0;
            int used = 0;
            for (int j = 0; j < order; j++) {
                if (i != j && edges[i][j] == 1) {
                    used++;
                } else if (i != j && edges[i][j] == 0) {
                    remaining++;
                }
            }

            if (used < 2) {
                if ((remaining == 2 && used == 0) || (remaining == 1 && used == 1)) {
                    for (int j = 0; j < order; j++) {
                        if (i != j && edges[i][j] == 0) {
                            edges[i][j] = 1;
                            edges[j][i] = 1;
                        }
                    }
                }
            } else {
                for (int j = 0; j < order; j++) {
                    if (i != j && edges[i][j] == 0) {
                        edges[i][j] = -1;
                        edges[j][i] = -1;
                    }
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