
# Robert Owens (rao7utn)
# 10/6/2020
# Makefile for post-lab (AVL Tree)
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-Wall -O2
DEBUG=
OBJECTS= AVLNode.o AVLPathTest.o AVLTree.o

AVLPathTest: $(OBJECTS)
	$(CXX) $(DEBUG) $(OBJECTS)

AVLNode.o: AVLNode.cpp AVLNode.h

AVLPathTest.o: AVLPathTest.cpp AVLTree.h AVLNode.h

AVLTree.o: AVLTree.cpp AVLNode.h AVLTree.h

.PHONY: clean
clean:
	-rm -f *.out AVLPathTest
	-rm -R *.out.dSYM
