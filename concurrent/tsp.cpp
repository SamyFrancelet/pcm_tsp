//
//  tsp.cpp
//  
//  Copyright (c) 2023 Landry Reynard & Samy Francelet.
//  All rights reserved.
//

#include "tsp.h"

static const struct {
	char RED[6];
	char BLUE[6];
	char ORIGINAL[6];
} COLOR = {
	.RED = { 27, '[', '3', '1', 'm', 0 },
	.BLUE = { 27, '[', '3', '6', 'm', 0 },
	.ORIGINAL = { 27, '[', '3', '9', 'm', 0 },
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

static ConcurrentStack<Path*> workToDo;

void startTSP(char* fname) {
    std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;

    Graph* g = TSPFile::graph(fname);
    if (global.verbose & VER_GRAPH)
        std::cout << COLOR.BLUE << g << COLOR.ORIGINAL << std::endl;

    global.shortest = new Path(g);
    for (int i = 0; i < g->size(); i++)
        global.shortest->add(i);

    global.shortest->add(0);

    begin = std::chrono::steady_clock::now();

    

    end = std::chrono::steady_clock::now();

	std::cout << "time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";
	std::cout << "time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "us\n";
}