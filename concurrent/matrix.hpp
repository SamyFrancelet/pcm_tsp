#include <iostream>

#ifndef MATRIX_HPP
#define MATRIX_HPP

class Matrix {
public:
    Matrix(int order) : _order(order) {
        _distanceMatrix = new int*[order];
        for (int i = 0; i < order; i++) {
            _distanceMatrix[i] = new int[order];
        }
    }

    int distance(int i, int j) const { return _distanceMatrix[i][j]; }
    int& sdistance(int i, int j) { return _distanceMatrix[i][j]; }

    int order() const { return _order; }
    int **matrix() const { return _distanceMatrix; }

    void display() {
        std::cout << "\t";
        for (int i = 0; i < _order; i++) {
            std::cout << i << "\t";
        }
        std::cout << std::endl;

        for (int i = 0; i < _order; i++) {
            std::cout << i << "\t";
            for (int j = 0; j < _order; j++) {
                std::cout << distance(i, j) << "\t";
            }
            std::cout << std::endl;
        }
        
    }

private:
    int _order;
    int **_distanceMatrix;
};

#endif // MATRIX_HPP