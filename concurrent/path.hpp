#include <vector>
#include <limits>
#include "matrix.hpp"

#ifndef PATH_HPP
#define PATH_HPP

typedef std::vector<std::vector<int>> EdgeMatrix;

class Path {
public:
    Path(Matrix *pMatrix, EdgeMatrix edgeMatrix) 
        : _pMatrix(pMatrix), _edgeMatrix(edgeMatrix) 
    {
        _lowerBound = find_lower_bound();
    }

    EdgeMatrix edge_matrix() {
        return _edgeMatrix;
    }

    double lower_bound() {
        return _lowerBound;
    }

        bool complete() {
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (i == j) continue;
                if (_edgeMatrix[i][j] == 0) return false;
            }
        }

        return true;
    }

    bool is_route() {
        int order = _pMatrix->order();
        std::vector<bool> visited(order, false);
        std::vector<int> used(order, 0);

        bool isCircular = false;
        int node = -1;

        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    used[i]++;
                }
            }
        }

        for (int i = 0; i < order; i++) {
            // If we didn't go in and out of the same vertex, then the graph
            // is not a route.
            if (used[i] != 2) return false;
        }

        for (int i = 0; (i <= order) || !isCircular; i++) {
            node = get_next_node(node, 0);

            if (visited[node]) {
                isCircular = true;
            } else {
                visited[node] = true;
            }
        }

        return true;
    }

    int cost() {
        if (!is_route()) return std::numeric_limits<int>::max();

        int order = _pMatrix->order();
        int c = 0;
        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 1)
                    c += _pMatrix->matrix()[i][j];
            }
        }

        return c;
    }

    void display() {
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            for (int j = i+1; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    std::cout << i << " -> " << j << std::endl;
                }
            }
        }
        std::cout << "Cost: " << cost() << std::endl;
    }

private:
    int get_next_node(int prev, int current) {
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            if (_edgeMatrix[current][i] == 1 && i != prev) {
                return i;
            }
        }
        return 0;
    }

    double find_lower_bound() {
        int **inputMatrix = _pMatrix->matrix();
        int order = _pMatrix->order();

        int min = std::numeric_limits<int>::max();
        int secondMin = std::numeric_limits<int>::max();

        int total = 0;
        bool set = false;

        for (int i = 0; i < order; i++) {
            int positivused = 0;

            for (int j = 0; j < order; j++) {
                if (positivused == 2) 
                {
                    // If including edge(i, j) would result in more than 2 used
                    // at vertex i, then edge(i, j) must be discarded.
                    break;
                } 
                else if (_edgeMatrix[i][j] == 1) 
                {
                    // If edge(i, j) is marked as included, then it must be
                    // included.
                    positivused++;
                    if (set) {
                        secondMin = inputMatrix[i][j];
                    } else {
                        min = inputMatrix[i][j];
                        set = true;
                    }
                }
                else if (inputMatrix[i][j] <= secondMin && inputMatrix[i][j] != 0
                        && _edgeMatrix[i][j] != -1) 
                {
                    // If edge(i, j) is shorter than the current second shortest
                    // edge, then it must be included.
                    if (set) {
                        if (min < secondMin) 
                            secondMin = min;
                        
                        min = inputMatrix[i][j];
                    } else {
                        secondMin = inputMatrix[i][j];
                    }
                }
            }

            set = false;

            // If we have not found two used at vertex i, then the graph is not
            // complete and we can return 0.0.
            if (min == std::numeric_limits<int>::max() || secondMin == std::numeric_limits<int>::max())
                return 0.0;

            total += min + secondMin;
            min = std::numeric_limits<int>::max();
            secondMin = std::numeric_limits<int>::max();
        }

        return total / 2.0;
    }

    Matrix *_pMatrix;
    EdgeMatrix _edgeMatrix;

    double _lowerBound;
};

#endif // PATH_HPP