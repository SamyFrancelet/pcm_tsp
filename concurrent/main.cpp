//
//  main.cpp
//  
//  Copyright (c) 2023 Landry Reynard & Samy Francelet.
//  All rights reserved.
//

#include "tsp.h"

int main(int argc, char* argv[])
{
	Verbosity verbose;
	// Parse command line
    char* fname = 0;
	if (argc == 2) {
		fname = argv[1];
		verbose = VER_NONE;
	} else {
		if (argc == 3 && argv[1][0] == '-' && argv[1][1] == 'v') {
			verbose = (Verbosity) (argv[1][2] ? atoi(argv[1]+2) : 1);
			fname = argv[2];
		} else {
			fprintf(stderr, "usage: %s [-v#] filename\n", argv[0]);
			exit(1);
		}
	}

	// Start the TSP
	startTSP(fname, verbose);
}