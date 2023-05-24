#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include <stack>
#include <queue>

#include "matrix.hpp"
#include "tspfile.hpp"
#include "path.hpp"
#include "bnb.hpp"

std::stack<Path> paths;

void solve(Matrix *pMatrix, Path best)
{
    std::chrono::steady_clock::time_point start, end;
    EdgeMatrix edgeMatrix(pMatrix->order(), std::vector<int>(pMatrix->order(), 0));
    Path root(pMatrix, edgeMatrix);
    paths.push(root);

    start = std::chrono::steady_clock::now();
    while (!paths.empty()) {
        Path path = paths.top();
        paths.pop();

        if (path.valid() && path.complete()) {
            if (path.cost() < best.cost()) {
                best = path;
            }
        }

        if (path.valid() && !path.complete()) {
            BnB bnb(path.edge_matrix());
            Path left(pMatrix, bnb.left());
            Path right(pMatrix, bnb.right());

            if (left.lower_bound() <= best.cost()) {
                paths.push(left);
            }

            if (right.lower_bound() <= best.cost()) {
                paths.push(right);
            }
        }
    }
    end = std::chrono::steady_clock::now();

    std::cout << "Best path:" << std::endl;
    best.display();

    std::cout << "Time elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
}

int main(int argc, char* argv[]) {
    Matrix *matrix;
    int nThreads = 1;

    if (argc == 2) {
        std::string tspFile(argv[1]);
        matrix = TSPFile::matrix(tspFile);
    } else if (argc == 1) {
        int defaultMatrix[5][5] =  {{0, 3, 4, 2, 7},
                                    {3, 0, 4, 6, 3},
                                    {4, 4, 0, 5, 8},
                                    {2, 6, 5, 0, 6},
                                    {7, 3, 8, 6, 0}};
        matrix = new Matrix(5);

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                matrix->sdistance(i, j) = defaultMatrix[i][j];
            }
        }
    } else if (argc == 3) {
        std::string tspFile(argv[1]);
        matrix = TSPFile::matrix(tspFile);
        nThreads = std::stoi(argv[2]);
    } else {
        std::cout << "Usage: " << argv[0] << " <tsp file> <n threads=1>" << std::endl;
        return 1;
    }

    //std::cout << "Matrix order: " << matrix->order() << std::endl;
    //matrix->display();

    // Generate initial path
    EdgeMatrix edgeMatrix(matrix->order(), std::vector<int>(matrix->order(), -1));

    // Generate initial path 0 -> 1 -> 2 -> ... -> n -> 0
    // EdgeMatrix will look like this:
    //      0    1    2    3    4
    // 0    0    1    -1   -1   1
    // 1    1    0    1    -1   -1
    // 2    -1   1    0    1    -1
    // 3    -1   -1   1    0    1
    // 4    1    -1   -1   1    0

    for (int i = 0; i < matrix->order(); i++) {
        for (int j = 0; j < matrix->order(); j++) {
            if (i == j) {
                edgeMatrix[i][j] = 0;
            } else if (i == j+1 || i+1 == j) {
                edgeMatrix[i][j] = 1;
            } else if ((i == 0 && j == matrix->order()-1) || (i == matrix->order()-1 && j == 0)) {
                edgeMatrix[i][j] = 1;
            } else {
                edgeMatrix[i][j] = -1;
            }
        }
    }

    Path initialPath(matrix, edgeMatrix);
    //initialPath.display();

    solve(matrix, initialPath);

    return 0;
}