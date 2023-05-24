//
//  tsp.cpp
//  
//  Copyright (c) 2022 Marcelo Pasin. All rights reserved.
//

#include "../sequential/graph.hpp"
#include "../sequential/path.hpp"
#include "../sequential/tspfile.hpp"
#include "containers/containers.hpp"
#include "containers/atomic.hpp"
#include <chrono>


enum Verbosity {
    VER_NONE = 0,
    VER_GRAPH = 1,
    VER_SHORTER = 2,
    VER_BOUND = 4,
    VER_ANALYSE = 8,
    VER_COUNTERS = 16,
};

static struct {
    Verbosity verbose;
    struct {
        int* bound; // # of bound operations per level
    } counter;
    int size;
    int total;      // number of paths to check
    int* fact;
} global;

Container container;

static const struct {
    char RED[6];
    char BLUE[6];
    char ORIGINAL[6];
} COLOR = {
    .RED = { 27, '[', '3', '1', 'm', 0 },
    .BLUE = { 27, '[', '3', '6', 'm', 0 },
    .ORIGINAL = { 27, '[', '3', '9', 'm', 0 },
};


static void branch_and_bound(Path* current)
{
    if (global.verbose & VER_ANALYSE)
        std::cout << "analysing " << current << '\n';

    if (current->leaf()) {
        // this is a leaf
        current->add(0);
        if (global.verbose & VER_COUNTERS)
            container.set_verified_path(new int(container.get_verified_path() + 1));
        if (current->distance() < container.get_path().distance()) {
            if (global.verbose & VER_SHORTER)
                std::cout << "shorter: " << current << '\n';
            container.set_path(current);
            if (global.verbose & VER_COUNTERS)
                container.set_verified_path(new int(container.get_verified_path() + 1));
        }
        current->pop();
    } else {
        // not yet a leaf
        if (current->distance() < container.get_path().distance()) {
            // continue branching
            for (int i=1; i<current->max(); i++) {
                if (!current->contains(i)) {
                    current->add(i);
                    branch_and_bound(current);
                    current->pop();
                }
            }
        } else {
            // current already >= shortest known so far, bound
            if (global.verbose & VER_BOUND )
                std::cout << "bound " << current << '\n';
            if (global.verbose & VER_COUNTERS)
                global.counter.bound[current->size()] ++;
        }
    }
}


void reset_counters(int size)
{
    global.size = size;
    container.set_verified_path(new int(0));
    container.set_path(new Path(nullptr));
    for (int i=0; i<global.size; i++) {
        global.counter.bound[i] = 0;
        if (i) {
            int pos = global.size - i;
            global.fact[pos] = (i-1) ? (i * global.fact[pos+1]) : 1;
        }
    }
    global.total = global.fact[0] = global.fact[1];
}

void print_counters()
{
    std::cout << "total: " << global.total << '\n';
    std::cout << "verified: " << container.get_verified_path() << '\n';
    std::cout << "found shorter: " << container.get_verified_path() << '\n';
    std::cout << "bound (per level):";
    for (int i=0; i<global.size; i++)
        std::cout << ' ' << global.counter.bound[i];
    std::cout << "\nbound equivalent (per level): ";
    int equiv = 0;
    for (int i=0; i<global.size; i++) {
        int e = global.fact[i] * global.counter.bound[i];
        std::cout << ' ' << e;
        equiv += e;
    }
    std::cout << "\nbound equivalent (total): " << equiv << '\n';
    std::cout << "check: total " << (global.total==(container.get_verified_path() + equiv) ? "==" : "!=") << " verified + total bound equivalent\n";
}

int main(int argc, char* argv[])
{
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;

    char* fname = 0;
    if (argc == 2) {
        fname = argv[1];
        global.verbose = VER_NONE;
    } else {
        if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'v') {
            global.verbose = (Verbosity) (argv[1][2] ? atoi(argv[1]+2) : 1);
            fname = argv[2];
        } else {
            fprintf(stderr, "usage: %s [-v#] filename\n", argv[0]);
            exit(1);
        }
    }

    Graph* g = TSPFile::graph(fname);
    if (global.verbose & VER_GRAPH)
        std::cout << COLOR.BLUE << g << COLOR.ORIGINAL;

    if (global.verbose & VER_COUNTERS)
        reset_counters(g->size());

    container.set_path(new Path(g));
    for (int i=0; i<g->size(); i++) {
        container.get_path().add(i);
    }
    container.get_path().add(0);

    begin = std::chrono::steady_clock::now();
    Path* current = new Path(g);
    current->add(0);
    branch_and_bound(current);
    end = std::chrono::steady_clock::now();

    container.print_path();

    if (global.verbose & VER_COUNTERS)
        print_counters();

    std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";
    std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us\n";

    return 0;
}
