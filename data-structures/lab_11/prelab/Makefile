# Robert Owens (rao7utn)
# 11/16/2020
# Makefile for Pre-lab 11
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-O2
OBJECTS= middleearth.o topological.o classnode.o

GEN_DOCS_COMMAND=doxygen


topological_sort: $(OBJECTS)
	$(CXX) $(OBJECTS)
	$(GEN_DOCS_COMMAND)


middleearth.o: middleearth.cpp middleearth.h
topological.o: topological.cpp classnode.h
classnode.o: classnode.cpp classnode.h

.PHONY: clean
clean:
	-rm -f *.out *.o
	-rm -R ./doc/html