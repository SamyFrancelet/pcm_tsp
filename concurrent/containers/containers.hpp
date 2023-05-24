#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include "atomic.hpp"
#include "../../sequential/path.hpp"
#include <iostream>

const int SIZE = 10;


class Container {
private:
    // Flag to indicate if the shortest path has been found
    atomic_stamped<bool> finished;
    // Shortest path found so far
    atomic_stamped<Path> shortestPath;
    // Verified path
    atomic_stamped<int> verifiedPath;
    // Table of thread statuses
    int* threadStatusTable[SIZE];
    atomic_stamped<int*> threadStatus;

public:
    Container() : finished(0, 0), shortestPath(nullptr, 0), verifiedPath(0, 0), threadStatus(threadStatusTable, 0) {}

    /** finished **/
    void set_finished(bool* value) {

        uint64_t stamp = 0;
        bool* expected = finished.get(stamp);
        while (!finished.cas(expected, value, stamp, stamp + 1)) {
            expected = finished.get(stamp);
        }
    }

    bool get_finished() {
        uint64_t stamp = 0;
        return finished.get(stamp);
    }


    /** shortestPath **/
    void set_path(Path* path) {
        uint64_t stamp = 0;
        Path* expected = shortestPath.get(stamp);
        while (!shortestPath.cas(expected, path, stamp, stamp + 1)) {
            expected = shortestPath.get(stamp);
        }
    }

    Path get_path() {
        uint64_t stamp = 0;
        return *shortestPath.get(stamp);
    }

    void print_path() {
        uint64_t stamp = 0;
        Path* path = shortestPath.get(stamp);

        std::cout << '[' << path->distance();
		for (int i=0; i<path->size(); i++)
			std::cout << (i?',':':') << ' ' << path->at(i);
		std::cout << ']';
    }

    std::ostream& print(std::ostream& os, Path* path) {
        path->print(os);
        return os;
    }

    /** verifiedPath **/
    void set_verified_path(int* value) {
        uint64_t stamp = 0;
        int* expected = verifiedPath.get(stamp);
        while (!verifiedPath.cas(expected, value, stamp, stamp + 1)) {
            expected = verifiedPath.get(stamp);
        }
    }
    
    int get_verified_path() {
        uint64_t stamp = 0;
        return *verifiedPath.get(stamp);
    }

    /** threadStatus **/
    void set_thread_status_table(int* value) {
        uint64_t stamp = 0;
        int** ptr = threadStatus.get(stamp);
        for (int i = 0; i < SIZE; i++) {
            ptr[i] = new int(value[i]);
        }
    }

    void set_thread_status(int i, int* value) {
        uint64_t stamp = 0;
        int** ptr = threadStatus.get(stamp);
        ptr[i] = value;
    }

    int* get_thread_status_table() {
        uint64_t stamp = 0;
        return *threadStatus.get(stamp);
    }

    int get_thread_status(int i) {
        uint64_t stamp = 0;
        int** status = threadStatus.get(stamp);
        return *status[i];
    }

    void print_thread_status() {
        uint64_t stamp = 0;
        int i;
        int** ptr = threadStatus.get(stamp);
        for (i = 0; i < SIZE; i++) {
            std::cout << *ptr[i] << " ";
        }
        std::cout << std::endl;
    }

};

#endif