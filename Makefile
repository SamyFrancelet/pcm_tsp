#  Copyright (c) 2012 Marcelo Pasin. All rights reserved.

CFLAGS=-O3 -Wall
LDFLAGS=-O3 -lm

all: tspcc 

tspcc: sequential/tspcc.o
	c++ -o tspcc $(LDFLAGS) sequential/tspcc.o

tspcc.o: sequential/tspcc.cpp sequential/graph.hpp sequential/path.hpp sequential/tspfile.hpp
	c++ $(CFLAGS) -c sequential/tspcc.cpp

omp:
	make tspcc CFLAGS="-fopenmp -O3" LDFLAGS="-fopenmp -O3"

clean:
	rm -f sequential/*.o sequential/tspcc

test_stack:
	c++ -o concurrent/containers/test_stack concurrent/containers/test_stack.cpp -latomic -lpthread

concu: tsp

tsp: concurrent/tsp.o
	c++ -o tsp $(LDFLAGS) concurrent/tsp.o -latomic -lpthread

tsp.o: concurrent/tsp.cpp concurrent/containers/containers.hpp sequential/graph.hpp sequential/path.hpp sequential/tspfile.hpp
	c++ $(CFLAGS) -c sequential/tsp.cpp -latomic -lpthread
