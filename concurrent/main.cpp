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

std::mutex bestPath;
std::mutex tsp;

std::stack<Path> paths;

void thinking(Path *best, Matrix *pMatrix, bool *keepWaiting, int tid, int *threadStatus)
{
    while (*keepWaiting) {
        tsp.lock();
        if (paths.empty()) {
            threadStatus[tid] = 0; // I'm free !
            // Check if all threads are done
            int status = 0;
            for (int i = 0; i < sizeof(threadStatus); i++) {
                status += threadStatus[i];
            }

            if (status == 0) {
                *keepWaiting = false;
            }
            tsp.unlock();
            continue;
        }

        threadStatus[tid] = 1; // I'm working hard here !
        Path path = paths.top();
        paths.pop();

        tsp.unlock();

        if (path.is_route()) {
            if (path.cost() <= best->cost()) {
                bestPath.lock();
                *best = path;
                bestPath.unlock();
            }
        }

        if (!path.complete()) {
            BnB bnb(path.edge_matrix());
            Path leftChildPath(pMatrix, bnb.left());
            Path rightChildPath(pMatrix, bnb.right());

            double leftLowerBound = leftChildPath.lower_bound();
            tsp.lock();
            if (leftLowerBound <= best->cost()) {
                // If the left path lower bound is less than the best path cost,
                // then the left path is a candidate for the best path.
                paths.push(leftChildPath);
            }
            tsp.unlock();

            double rightLowerBound = rightChildPath.lower_bound();
            tsp.lock();
            if (rightLowerBound <= best->cost()) {
                // If the right path lower bound is less than the best path cost,
                // then the right path is a candidate for the best path.
                paths.push(rightChildPath);
            }
            tsp.unlock();
        }
    }
}

void start_tsp(Matrix *pMatrix) {
    std::chrono::steady_clock::time_point begin, end;

    int order = pMatrix->order();
    EdgeMatrix edges(order, std::vector<int>(order, 0));

    Path best(pMatrix, edges);
    paths.push(best);
    std::cout << "Initial best path: " << std::endl;
    best.display();

    unsigned int nCores = std::thread::hardware_concurrency();
    unsigned int nThreads;

    std::cout << "Number of possible threads: " << nCores << std::endl;
    std::cout << "Please enter the number of threads to use: ";
    std::cin >> nThreads;

    if (nThreads > nCores) {
        std::cout << "Number of threads requested is greater than the number of cores available. Using "
                  << nCores << " threads." << std::endl;
        nThreads = nCores;
    }

    std::thread threads[nThreads];
    int threadStatus[1000];
    for (int i = 0; i < 1000; i++) {
        threadStatus[i] = i < nThreads ? 1 : 0; // 1 = running, 0 = no more work
    }

    bool keepWaiting = true;

    begin = std::chrono::steady_clock::now();
    for (int i = 0; i < nThreads; i++) {
        threads[i] = std::thread(thinking, &best, pMatrix, &keepWaiting, i, threadStatus);
    }

    for (int i = 0; i < nThreads; i++) {
        threads[i].join();
    }
    end = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsedSeconds = end - begin;
    best.display();
    std::cout << "Elapsed time: " << elapsedSeconds.count() << " seconds" << std::endl;
}

int main(int argc, char* argv[]) {
    Matrix *matrix;

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
    } else {
        std::cout << "Usage: ./tsp <tsp file>" << std::endl;
        return 1;
    }

    std::cout << "Matrix order: " << matrix->order() << std::endl;
    matrix->display();
    start_tsp(matrix);

    return 0;
}