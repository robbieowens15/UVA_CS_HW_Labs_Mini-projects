# Robert Owens (rao7utn)
# 10/12/2020
# Makefile for lab-6
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-O2
DEBUG=
OBJECTS= wordPuzzle.o Hashtable.o timer.o

wordPuzzle: $(OBJECTS)
	$(CXX) $(DEBUG) $(OBJECTS)

wordPuzzle.o: wordPuzzle.cpp Hashtable.h

Hashtable.o: Hashtable.cpp Hashtable.h

timer.o: timer.cpp timer.h

.PHONY: clean
clean:
	-rm -f *.out *.o wordPuzzle
