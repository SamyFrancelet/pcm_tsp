#ifndef TSP_H
#define TSP_H

#include <chrono>
#include "graph.hpp"
#include "path.hpp"
#include "tspfile.hpp"

#include "containers/stack.hpp"

enum Verbosity {
	VER_NONE = 0,
	VER_GRAPH = 1,
	VER_SHORTER = 2,
	VER_BOUND = 4,
	VER_ANALYSE = 8,
	VER_COUNTERS = 16,
};

void startTSP(char* fname, Verbosity verbose);

#endif // TSP_H