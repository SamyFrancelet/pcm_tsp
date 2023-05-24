#include <vector>
#include <limits>
#include "matrix.hpp"

#ifndef PATH_HPP
#define PATH_HPP

/**
 * The edge matrix is a matrix that represents the state of the edges of a path.
 * If the edge matrix at position [i][j] is 1, then the edge (i, j) is in the path.
 * If the edge matrix at position [i][j] is 0, then the edge (i, j) is unused.
 * If the edge matrix at position [i][j] is -1, then the edge (i, j) is excluded from the path.
*/
typedef std::vector<std::vector<int>> EdgeMatrix;


/**
 * The Path class represents a path in the graph.
 * It contains the edge matrix, the lower bound, the cost, and the status of the path.
 * The status of the path is either complete or not complete.
 * A path is complete if all the edges are used.
*/
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

        int prev = -1;
        int curr = 0;
        int next = 0;
        int count = 0;

        bool isCircular = false;

        while (true) {
            next = get_next_node(prev, curr);
            prev = curr;
            curr = next;
            count++;

            if (curr == 0) {
                // Back to start !
                if (count == order) isCircular = true;
                break;
            }
        }

        return isCircular;
    }

    int cost() {
        return _cost;
    }

    int find_cost() {
        int order = _pMatrix->order();
        int c = 0;
        for (int i = 0; i < order; i++) {
            for (int j = i + 1; j < order; j++) {
                if (_edgeMatrix[i][j] == 1)
                    c += _pMatrix->matrix()[i][j];
            }
        }

        return c;
    }

    void display() {
        int order = _pMatrix->order();
        std::cout << "This path includes the following edges: " << std::endl;
        for (int i = 0; i < order; i++) {
            for (int j = i+1; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    std::cout << i << " <-> " << j << std::endl;
                }
            }
        }

        // Print path starting from 0 and following the edges.
        int next = 0;
        int curr = 0;
        int prev = 0;

        std::cout << "[" << _cost << ": " << "0";
        for (int i = 0; i < order; i++) {
            next = get_next_node(prev, curr);
            prev = curr;
            curr = next;
            std::cout << " -> " << next;
        }

        std::cout << "]" << std::endl;
    }

private:
    /**
     * Returns the next node in the path.
     * Next node is the node that is connected to the current node
     * and is not the previous node.
    */
    int get_next_node(int prev, int current) {
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            if (_edgeMatrix[current][i] == 1 && i != prev) {
                return i;
            }
        }
        return 0;
    }

    /**
     * Finds the lower bound of the path 
     * by finding the two smallest edges for each vertex
     * or counting already used edges.
     * @return The lower bound of the path.
    */
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
                // If we have found two used at vertex i, then we can break
                // out of the loop.
                if (positiveEdges >= 2) break;

                if (_edgeMatrix[i][j] == 1 && !set) {
                    // If vertex i j is used, then we set the min to the value
                    // of the edge and set the flag to true.
                    min = inputMatrix[i][j];
                    positiveEdges++;
                    set = true;
                } else if (_edgeMatrix[i][j] == 1 && set) {
                    // If we have already set the min, then we set the second
                    // min to the value of the edge.
                    secondMin = inputMatrix[i][j];
                    positiveEdges++;
                } else if (inputMatrix[i][j] <= min && inputMatrix[i][j] != 0 && _edgeMatrix[i][j] != -1 && !set) {
                    if (min < secondMin) {
                        secondMin = min;
                    }
                    min = inputMatrix[i][j];
                } else if (inputMatrix[i][j] < secondMin && inputMatrix[i][j] != 0 && _edgeMatrix[i][j] != -1) {
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