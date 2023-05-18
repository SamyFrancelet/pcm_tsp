//
//  main.cpp
//  
//  Copyright (c) 2023 Landry Reynard & Samy Francelet.
//  All rights reserved.
//

#include <chrono>
#include "graph.hpp"
#include "path.hpp"
#include "tspfile.hpp"

static const struct {
	char RED[6];
	char BLUE[6];
	char ORIGINAL[6];
} COLOR = {
	.RED = { 27, '[', '3', '1', 'm', 0 },
	.BLUE = { 27, '[', '3', '6', 'm', 0 },
	.ORIGINAL = { 27, '[', '3', '9', 'm', 0 },
};

enum Verbosity {
	VER_NONE = 0,
	VER_GRAPH = 1,
	VER_SHORTER = 2,
	VER_BOUND = 4,
	VER_ANALYSE = 8,
	VER_COUNTERS = 16,
};

static struct {
	Path* shortest;
	Verbosity verbose;
	struct {
		int verified;	// # of paths checked
		int found;	// # of times a shorter path was found
		int* bound;	// # of bound operations per level
	} counter;
	int size;
	int total;		// number of paths to check
	int* fact;
} global;

int main(int argc, char* argv[])
{
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

	// Parse command line
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

	// Read graph from file
	Graph* g = TSPFile::graph(fname);
	if (global.verbose & VER_GRAPH)
		std::cout << COLOR.BLUE << g << COLOR.ORIGINAL;

	/*if (global.verbose & VER_COUNTERS)
		reset_counters(g->size());*/

	// Set initial shortest path (0 -> 1 -> 2 -> ... -> n -> 0)
	global.shortest = new Path(g);
	for (int i=0; i<g->size(); i++) {
		global.shortest->add(i);
	}
	global.shortest->add(0);

    begin = std::chrono::steady_clock::now();

    // compute shortest path here

    end = std::chrono::steady_clock::now();

    std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";
	std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us\n";
}