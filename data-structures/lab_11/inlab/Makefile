# Robert Owens (rao7utn)
# 11/17/2020
# Makefile for In-lab 11
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-O2
OBJECTS=middleearth.o traveling.o

GEN_DOCS_COMMAND=doxygen

compile: $(OBJECTS)
	$(CXX) $(OBJECTS)
	$(GEN_DOCS_COMMAND)

middleearth.o: middleearth.cpp middleearth.h
traveling.o: traveling.cpp middleearth.h

.PHONY: clean
clean:
	-rm -f *.out *.o
	-rm -R ./doc/html