#  Copyright (c) 2012 Marcelo Pasin. All rights reserved.

CFLAGS=-O3 -Wall -pthread
LDFLAGS=-O3 -lm

all: tspcc tspmt

tspmt: concurrent/main.o
	c++ -o tspmt $(LDFLAGS) concurrent/main.o

main.o: concurrent/main.cpp concurrent/matrix.hpp concurrent/tspfile.hpp concurrent/path.hpp concurrent/bnb.hpp
	c++ $(CFLAGS) -c concurrent/main.cpp

tspcc: sequential/tspcc.o
	c++ -o tspcc $(LDFLAGS) sequential/tspcc.o

tspcc.o: sequential/tspcc.cpp sequential/graph.hpp sequential/path.hpp sequential/tspfile.hpp
	c++ $(CFLAGS) -c sequential/tspcc.cpp

omp:
	make tspcc CFLAGS="-fopenmp -O3" LDFLAGS="-fopenmp -O3"

clean:
	rm -f sequential/*.o tspcc
	rm -f concurrent/*.o tspmt
