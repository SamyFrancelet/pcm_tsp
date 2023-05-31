#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstdint>

#include <stack>
#include <queue>

#include "matrix.hpp"
#include "tspfile.hpp"
#include "path.hpp"
#include "bnb.hpp"
#include "containers/stack.hpp"
#include "containers/c_object.hpp"

// Create a struct containing a bool and a table of 300 ints
struct Status {
    bool keepRunning;
    int threadStatus[300];
};

ConcurrentStack<Path*> paths;
CObject<Status> runningStatus;

CObject<Path> best;

void solve(Matrix *pMatrix, int tid)
{
    Path * path = nullptr;
    while (runningStatus.get()->keepRunning) {

        path = paths.pop();

        if (path == nullptr) {
            runningStatus.get()->threadStatus[tid] = 0; // I'm free!

            int status = 0;
            for (int i = 0; i < 300; i++) {
                status += runningStatus.get()->threadStatus[i];
            }

            if (status == 0) {
                runningStatus.get()->keepRunning = false;
            }
            continue;
        }
        runningStatus.get()->threadStatus[tid] = 1; // I'm enslaved...

        if (path->valid() && path->complete()) {
            if (path->cost() <= best.get()->cost()) {
                best.set(path);
                continue;
            }
        }

        if (path->valid() && !path->complete()) {
            BnB bnb(path->edge_matrix());
            Path *left = new Path(pMatrix, bnb.left());
            Path *right = new Path(pMatrix, bnb.right());

            if (left->lower_bound() <= best.get()->cost()) {
                paths.push(left);
            } else {
                delete left;
            }

            if (right->lower_bound() <= best.get()->cost()) {
                paths.push(right);
            } else {
                delete right;
            }
        }

        delete path;
        path = nullptr;
    }
}

void start_tsp(Matrix *pMatrix, int nThreads) {
    std::chrono::steady_clock::time_point start, end;

    EdgeMatrix rootEdges(pMatrix->order(), std::vector<int>(pMatrix->order(), 0));
    Path *root = new Path(pMatrix, rootEdges);
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

    Path *path = new Path(pMatrix, edgeMatrix);
    best.set(path);
    std::thread threads[nThreads];
    for (int i = 0; i < 300; i++) {
        // 1 = running, 0 = stopped
        runningStatus.get()->threadStatus[i] = i < nThreads ? 1 : 0;
    }
    runningStatus.get()->keepRunning = true;
    start = std::chrono::steady_clock::now();
    for (int i = 0; i < nThreads; i++) {
        threads[i] = std::thread(solve, pMatrix, i);
    }
    for (int i = 0; i < nThreads; i++) {
        threads[i].join();
    }
    end = std::chrono::steady_clock::now();

    //std::cout << "Best path: ";
    best.get()->display();
    //std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
    //std::cout << (paths.empty() ? "Empty" : "NOT EMPTY ????????") << std::endl;
    std::chrono::duration<double>elapsedSeconds = end - start;
    std::cout<<nThreads<<";"<<elapsedSeconds.count()<<std::endl;
}

int main(int argc, char* argv[]) {
    Matrix *matrix;
    int nThreads = 1;
    // Create and set status
    Status* statusTemp = new Status();
    statusTemp->keepRunning = true;
    for (int i = 0; i < 300; i++) {
        statusTemp->threadStatus[i] = 0;
    }
    runningStatus.set(statusTemp);

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
