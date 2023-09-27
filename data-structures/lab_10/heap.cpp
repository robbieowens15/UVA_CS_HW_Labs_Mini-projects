/*
Robert Owens (rao7utn)
11/8/2020
heap.cpp
*/

/*
Source: https://uva-cs.github.io/pdr/slides/code/10-heaps-huffman/binary_heap.cpp.html
Implimentation was heavily influenced by the provided file
*/
#include <iostream>
#include "heap.h"

Heap::Heap(){
	array.push_back(NULL);
	size = 0;
}

Heap::~Heap(){
}

void Heap::insert(char value, unsigned int freq){
	HuffmanNode* newNode = new HuffmanNode(value, freq);
	insert(newNode);
}

void Heap::insert(HuffmanNode* node){
	array.push_back(node);
	percolateUp(++size);
}

HuffmanNode* Heap::findMin(){
	if (size == 0){
		cout << "findMin() called on empty heap";
		return NULL;
	}
	return array[1];
}

HuffmanNode* Heap::deleteMin(){
	if (size == 0){
		cout << "findMin() called on empty heap";
		return NULL;
	}
	HuffmanNode* toReturn = array[1];
	array[1] = array[size--];
	array.pop_back();
	if(!(size == 0)){
		percolateDown(1);
	}

	return toReturn;
}

void Heap::percolateUp(int hole){
	HuffmanNode* toMove = array[hole];
	for( ; (hole > 1) && (toMove->frequency < array[hole/2]->frequency); hole /= 2){
		array[hole] = array[hole/2]; //move parent down
	}
	array[hole] = toMove; //inserted in correct position
}

void Heap::percolateDown(int hole){
	HuffmanNode* toMove = array[hole];
	while(hole*2 <= size){
		int childIndex = hole*2; // start with left child
		if((childIndex+1 <= size) 
			&& (array[childIndex+1]->frequency < array[childIndex]->frequency)){
			childIndex++; // if right child exists and is lesser, then swap with right child
		}
		if (toMove->frequency > array[childIndex]->frequency){
			array[hole] = array[childIndex]; //move child up
			hole = childIndex; //move hole down
		}
		else {
			break;
		}
	}
	array[hole] = toMove;
}

void Heap::print() {
    for (int i = 1; i <= size; i++) {
        array[i]->print(); 
        cout << " ";
        // next line from http://tinyurl.com/mf9tbgm
        bool isPow2 = (((i+1) & ~(i))==(i+1))? i+1 : 0;
        if (isPow2) {
            cout << endl << "\t";
        }
    }
    cout << endl;
}

int Heap::getSize(){
	return size;
}