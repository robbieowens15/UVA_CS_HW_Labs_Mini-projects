# Robert Owens (rao7utn)
# 10/3/2020
# Makefile for in-lab (binarySearchTree)
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-Wall -O2
DEBUG=
OBJECTS= BSTPathTest.o BinaryNode.o BinarySearchTree.o

BSTPathTest: $(OBJECTS)
	$(CXX) $(DEBUG) $(OBJECTS)

BSTPathTest.o: BSTPathTest.cpp BinarySearchTree.h BinaryNode.h

BinaryNode.o: BinaryNode.cpp BinaryNode.h

BinarySearchTree.o: BinarySearchTree.cpp BinaryNode.h BinarySearchTree.h

.PHONY: clean
clean:
	-rm -f *.out $(OBJECTS)
