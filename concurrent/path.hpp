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
    Path(Matrix *matrix, EdgeMatrix edgeMatrix)
        : _pMatrix(matrix), _edgeMatrix(edgeMatrix)
    {
        _valid = is_valid();
        if (_valid) {
            _lowerBound = get_lower_bound();
            _complete = is_complete();
            if (_complete) {
                _cost = get_cost();
            } else {
                _cost = std::numeric_limits<int>::max();
            }
        } else {
            _lowerBound = std::numeric_limits<int>::max();
        }
    }

    bool valid() const { return _valid; }
    int lower_bound() const { return _lowerBound; }
    int cost() const { return _cost; }
    bool complete() const { return _complete; }

    EdgeMatrix edge_matrix() const { return _edgeMatrix; }

    void display() {
        /*std::cout << "Edge matrix:" << std::endl;
        for (int i = 0; i < _pMatrix->order(); i++) {
            for (int j = 0; j < _pMatrix->order(); j++) {
                std::cout << _edgeMatrix[i][j] << "\t";
            }
            std::cout << std::endl;
        }
        std::cout << "Valid: " << (_valid ? "Yes" : "No") << std::endl;
        std::cout << "Lower bound: " << _lowerBound << std::endl;
        std::cout << "Complete: " << (_complete ? "Yes" : "No") << std::endl;
        std::cout << "Cost: " << _cost << std::endl;*/


        if (_valid && _complete) {
            int prev = 0;
            int curr = 0;
            int next = 0;

            //std::cout << "This path is:" << std::endl;
            std::cout << "[" << _cost << ": " << curr;
            for (int i = 0; i < _pMatrix->order(); i++) {
                next = get_next_node(prev, curr);
                prev = curr;
                curr = next;
                std::cout << " -> " << curr;
            }
            std::cout << "]" << std::endl;
        }
    }


private:
    /**
     * Check if the edge matrix is valid.
     * The edge matrix is valid if it contains only 0, 1, and -1.
     * and if there is maximum 2 1s per row and per column.
     * @return true if the edge matrix is valid, false otherwise.
    */
    bool is_valid()
    {
        int order = _pMatrix->order();

        // Check if the edge matrix contains only 0, 1, and -1.
        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] != 0 && _edgeMatrix[i][j] != 1 && _edgeMatrix[i][j] != -1) {
                    return false;
                }
            }
        }

        // Check if there is maximum 2 1s per row.
        for (int i = 0; i < order; i++) {
            int count = 0;
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    count++;
                }
            }
            if (count > 2) {
                return false;
            }
        }

        // Check if there is maximum 2 1s per column.
        for (int j = 0; j < order; j++) {
            int count = 0;
            for (int i = 0; i < order; i++) {
                if (_edgeMatrix[i][j] == 1) {
                    count++;
                }
            }
            if (count > 2) {
                return false;
            }
        }

        // Check if there is at least 2 1s or 0s per row.
        for (int i = 0; i < order; i++) {
            int count = 0;
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 1 || (_edgeMatrix[i][j] == 0 && i != j)) {
                    count++;
                }
            }
            if (count < 2) {
                return false;
            }
        }

        // Check if there is at least 2 1s or 0s per column.
        for (int j = 0; j < order; j++) {
            int count = 0;
            for (int i = 0; i < order; i++) {
                if (_edgeMatrix[i][j] == 1 || (_edgeMatrix[i][j] == 0 && i != j)) {
                    count++;
                }
            }
            if (count < 2) {
                return false;
            }
        }

        // Check that there is no unwanted cycle.
        int prev = 0;
        int curr = 0;
        int next = 0;
        _nNodes = 0;
        
        while (true) {
            next = get_next_node(prev, curr);
            _nNodes++;
            if (next == -1 || next == 0) {
                break;
            }
            prev = curr;
            curr = next;
        }

        if (next == 0 && _nNodes != order) {
            return false;
        }

        return true;
    }

    /**
     * Compute the lower bound cost of the path.
     * The lower bound cost of the path is the sum of the weights of the edges
     * that are used in the path, plus the sum of the minimum weights of the edges
     * that are not used in the path.
     * @return the lower bound cost of the path.
    */
    int get_lower_bound()
    {
        int order = _pMatrix->order();
        int lowerBound = 0;

        int** inMatrix = _pMatrix->matrix();

        int min = std::numeric_limits<int>::max();
        int min2 = std::numeric_limits<int>::max();

        bool set = false;

        // Compute the sum of the minimum weights of the edges that are not used in the path.
        for (int i = 0; i < order; i++) {
            int positiveEdges = 0;

            for (int j = 0; j < order; j++) {
                if (positiveEdges == 2) {
                    break;
                }

                if (_edgeMatrix[i][j] == 1 && !set) {
                    positiveEdges++;
                    set = true;
                    min = inMatrix[i][j];
                } else if (_edgeMatrix[i][j] == 1 && set) {
                    positiveEdges++;
                    min2 = inMatrix[i][j];
                } else if (inMatrix[i][j] <= min && inMatrix[i][j] != 0 && _edgeMatrix[i][j] != -1 && !set) {
                    if (min < min2) {
                        min2 = min;
                    }
                    min = inMatrix[i][j];
                } else if (inMatrix[i][j] < min2 && inMatrix[i][j] != 0 && _edgeMatrix[i][j] != -1) {
                    min2 = inMatrix[i][j];
                }
            }

            set = false;

            if (min == std::numeric_limits<int>::max() && min2 == std::numeric_limits<int>::max()) {
                return 0;
            }

            lowerBound += min + min2;
            min = std::numeric_limits<int>::max();
            min2 = std::numeric_limits<int>::max();
        }

        lowerBound /= 2;

        return lowerBound;
    }

    /**
     * Compute the cost of the path.
     * The cost of the path is the sum of the weights of the edges.
     * @return the cost of the path.
    */
    int get_cost()
    {
        int order = _pMatrix->order();
        int cost = 0;

        for (int i = 0; i < order; i++) {
            for (int j = i+1; j < order; j++) {
                if (_edgeMatrix[i][j] == 1) {
                    cost += _pMatrix->matrix()[i][j];
                }
            }
        }

        return cost;
    }

    /**
     * Check if the path is complete.
     * A path is complete if all the edges are used
     * and the path is circular (the last node is connected to the first node).
    */
    bool is_complete()
    {
        int order = _pMatrix->order();

        // Check if all the edges are used.
        for (int i = 0; i < order; i++) {
            for (int j = 0; j < order; j++) {
                if (_edgeMatrix[i][j] == 0 && i != j) {
                    return false;
                }
            }
        }

        // Check if the path is circular.
        int prev = -1;
        int node = 0;
        int next = 0;
        int count = 0;
        bool circular = false;

        while (count < order) {
            next = get_next_node(prev, node);
            if (next == -1) {
                return false;
            }
            prev = node;
            node = next;
            count++;
            if (node == 0 && count == order) {
                circular = true;
                break;
            }
        }

        return circular;
    }

    /**
     * Get the next node of the path.
     * @param prev The previous node.
     * @param node The current node.
     * @return The next node or -1 if there is no next node.
    */
    int get_next_node(int prev, int node)
    {
        int order = _pMatrix->order();
        for (int i = 0; i < order; i++) {
            if (_edgeMatrix[node][i] == 1 && i != prev) {
                return i;
            }
        }
        return -1;
    }

    Matrix *_pMatrix;
    EdgeMatrix _edgeMatrix;

    bool _valid;
    int _lowerBound;
    int _cost;
    bool _complete;
    int _nNodes;
};

#endif // PATH_HPP