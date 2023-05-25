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

std::mutex myMutex;

void solve(Matrix *pMatrix, Path *best, std::stack<Path> *paths, bool *keepRunning, int tid, int *threadStatus)
{
    while (*keepRunning) {
        myMutex.lock();
        if (paths->empty()) {
            threadStatus[tid] = 0; // I'm free!

            int status = 0;
            for (int i = 0; i < sizeof(threadStatus); i++) {
                status += threadStatus[i];
            }

            if (status == 0) {
                *keepRunning = false;
            }
            myMutex.unlock();
            continue;
        }

        threadStatus[tid] = 1; // I'm enslaved...
        Path path = paths->top();
        paths->pop();
        myMutex.unlock();

        if (path.valid() && path.complete()) {
            myMutex.lock();
            if (path.cost() <= best->cost()) {
                *best = path;
            }
            myMutex.unlock();
        }

        if (path.valid() && !path.complete()) {
            BnB bnb(path.edge_matrix());
            Path left(pMatrix, bnb.left());
            Path right(pMatrix, bnb.right());

            myMutex.lock();
            if (left.lower_bound() <= best->cost()) {
                paths->push(left);
            }
            myMutex.unlock();

            myMutex.lock();
            if (right.lower_bound() <= best->cost()) {
                paths->push(right);
            }
            myMutex.unlock();
        }
    }
}

void start_tsp(Matrix *pMatrix, int nThreads) {
    std::stack<Path> paths;
    std::chrono::steady_clock::time_point start, end;

    EdgeMatrix rootEdges(pMatrix->order(), std::vector<int>(pMatrix->order(), 0));
    Path root(pMatrix, rootEdges);
    paths.push(root);

    // Generate initial path
    EdgeMatrix edgeMatrix(pMatrix->order(), std::vector<int>(pMatrix->order(), -1));

    // Generate initial path 0 -> 1 -> 2 -> ... -> n -> 0
    // EdgeMatrix will look like this:
    //      0    1    2    3    4
    // 0    0    1    -1   -1   1
    // 1    1    0    1    -1   -1
    // 2    -1   1    0    1    -1
    // 3    -1   -1   1    0    1
    // 4    1    -1   -1   1    0

    for (int i = 0; i < pMatrix->order(); i++) {
        for (int j = 0; j < pMatrix->order(); j++) {
            if (i == j) {
                edgeMatrix[i][j] = 0;
            } else if (i == j+1 || i+1 == j) {
                edgeMatrix[i][j] = 1;
            } else if ((i == 0 && j == pMatrix->order()-1) || (i == pMatrix->order()-1 && j == 0)) {
                edgeMatrix[i][j] = 1;
            } else {
                edgeMatrix[i][j] = -1;
            }
        }
    }

    Path bestPath(pMatrix, edgeMatrix);

    std::thread threads[nThreads];
    int threadStatus[300];
    for (int i = 0; i < 300; i++) {
        // 1 = running, 0 = stopped
        threadStatus[i] = i < nThreads ? 1 : 0;
    }

    bool keepRunning = true;

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < nThreads; i++) {
        threads[i] = std::thread(solve, pMatrix, &bestPath, &paths, &keepRunning, i, threadStatus);
    }

    for (int i = 0; i < nThreads; i++) {
        threads[i].join();
    }
    end = std::chrono::steady_clock::now();

    std::cout << "Best path: ";
    bestPath.display();
    std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    std::cout << (paths.empty() ? "Empty" : "NOT EMPTY ????????") << std::endl;
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

    start_tsp(matrix, nThreads);

    return 0;
}