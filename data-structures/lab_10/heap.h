/*
Robert Owens (rao7utn)
11/8/2020
heap.h
*/

#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include "huffmanNode.h"

using namespace std;

class Heap {
public:
	Heap();
	~Heap();

	void insert(char value, unsigned int freq);
	void insert(HuffmanNode* node);
	HuffmanNode* findMin();
	HuffmanNode* deleteMin();
	int getSize();
	void print();

private:
	vector<HuffmanNode*> array;
	unsigned int size;

	void percolateUp(int hole);
	void percolateDown(int hole);
};

#endif