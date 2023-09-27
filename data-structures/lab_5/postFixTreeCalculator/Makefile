# Robert Owens (rao7utn)
# 10/3/2020
# Makefile postfixTreeCalc (prelab)
CXX=clang++ $(CXXFLAGS)
CXXFLAGS=-Wall -O2
DEBUG=
OBJECTS= TreeCalcTest.o TreeCalc.o TreeNode.o

TreeCalcTest: $(OBJECTS)
	$(CXX) $(DEBUG) $(OBJECTS)

TreeCalcTest.o: TreeCalcTest.cpp TreeCalc.h TreeNode.h

TreeCalc.o: TreeCalc.cpp TreeCalc.h TreeNode.h

TreeNode.o: TreeNode.cpp TreeNode.h

.PHONY: clean
clean:
	-rm -f *.o *.out
