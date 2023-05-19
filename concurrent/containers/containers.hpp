#ifndef CONTAINERS_HPP
#define CONTAINERS_HPP

#include "atomic.hpp"
#include "../../sequential/path.hpp"
#include <iostream>


class Container {
private:
    atomic_stamped<bool> finished;
    atomic_stamped<Path> shortestPath;
    atomic_stamped<int> verifiedPath;


public:
    Container() : finished(0, 0), shortestPath(nullptr, 0), verifiedPath(0, 0) {
    }

    /** finished **/
    void setFinished(bool* value) {

        uint64_t stamp = 0;
        bool* expected = finished.get(stamp);
        while (!finished.cas(expected, value, stamp, stamp + 1)) {
            expected = finished.get(stamp);
        }
    }

    bool getFinished() {
        uint64_t stamp = 0;
        return finished.get(stamp);
    }

    /** shortestPath **/
    void setPath(Path* path) {
        uint64_t stamp = 0;
        Path* expected = shortestPath.get(stamp);
        while (!shortestPath.cas(expected, path, stamp, stamp + 1)) {
            expected = shortestPath.get(stamp);
        }
    }

    Path getPath() {
        uint64_t stamp = 0;
        return *shortestPath.get(stamp);
    }

    void printPath() {
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
    void setVerifiedPath(int* value) {
        uint64_t stamp = 0;
        int* expected = verifiedPath.get(stamp);
        while (!verifiedPath.cas(expected, value, stamp, stamp + 1)) {
            expected = verifiedPath.get(stamp);
        }
    }
    
    int getVerifiedPath() {
        uint64_t stamp = 0;
        return *verifiedPath.get(stamp);
    }
};

#endif