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
        _isComplete = check_is_complete();
        _isRoute = check_is_route();
        if (_isRoute) {
            _cost = find_cost();
        } else {
            _cost = std::numeric_limits<int>::max();
        }
    }

    EdgeMatrix edge_matrix() {
        return _edgeMatrix;
    }

    double lower_bound() {
        return _lowerBound;
    }

    bool complete() {
        return _isComplete;
    }

    bool check_is_complete() {
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
        return _isRoute;
    }

    bool check_is_route() {
        int order = _pMatrix->order();
        std::vector<bool> visited(order, false);
        std::vector<int> used(order, 0);

        bool isCircular = false;
        int node = -1;

        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    used[i] += 1;
                }
            }
        }

        for (int i = 0; i < order; i++) {
            // If we didn't go in and out of the same vertex, then the graph
            // is not a route.
            if (used[i] != 2) return false;
        }

        int i = 0;
        while (!isCircular || (i <= order)) {
            i++;
            node = get_next_node(node, 0);
            if (visited[node]) {
                isCircular = true;
            } else {
                visited[node] = true;
            }
        }

        return isCircular;
    }

    int cost() {
        return _cost;
    }

    int find_cost() {
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
        std::cout << "This path includes the following edges: " << std::endl;
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            for (int j = i+1; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    std::cout << i << " <-> " << j << std::endl;
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
            int positiveEdges = 0;

            for (int j = 0; j < order; j++) {
                if (positiveEdges > 2) break;

                if (_edgeMatrix[i][j] == 1 && !set) {
                    min = inputMatrix[i][j];
                    positiveEdges++;
                    set = true;
                } else if (_edgeMatrix[i][j] == 1 && set) {
                    secondMin = inputMatrix[i][j];
                    positiveEdges++;
                } else if (inputMatrix[i][j] <= min && inputMatrix[i][j] != 0 && _edgeMatrix[i][j] == -1 && !set) {
                    if (min < secondMin) {
                        secondMin = min;
                    }
                    min = inputMatrix[i][j];
                } else if (inputMatrix[i][j] <= min && inputMatrix[i][j] != 0 && _edgeMatrix[i][j] == -1) {
                    secondMin = inputMatrix[i][j];
                }
            }

            set = false;

            // If we have not found two used at vertex i, then the graph is not
            // complete and we can return 0.0.
            if (min == std::numeric_limits<int>::max() || secondMin == std::numeric_limits<int>::max())
                return 0;

            total += min + secondMin;
            min = std::numeric_limits<int>::max();
            secondMin = std::numeric_limits<int>::max();
        }

        return total / 2.0;
    }

    Matrix *_pMatrix;
    EdgeMatrix _edgeMatrix;

    double _lowerBound;
    bool _isComplete;
    bool _isRoute;
    int _cost;
};

#endif // PATH_HPP