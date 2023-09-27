/*
Robert Owens (rao7utn)
11/8/2020
huffmanenc.cpp
*/
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <unordered_map>
#include <string>

#include "heap.h"

using namespace std;

void writeCodes(unordered_map<char, string>& table, HuffmanNode* curNode, string path){
	if (curNode == NULL){
		return;
	}
	else if(curNode->isLeaf()){
		table[curNode->getValue()] = path;
	}
	else{
		writeCodes(table, curNode->getRight(), path + "1");
		writeCodes(table, curNode->getLeft(), path + "0");
	}
}

unordered_map<char, string> generateLookup(HuffmanNode* root){
	unordered_map<char, string> table;
	writeCodes(table, root, "");
	return table;
}

HuffmanNode* buildHuffmanTree(Heap heap){
	while(heap.getSize() > 1) {
		HuffmanNode* a = heap.deleteMin();
		HuffmanNode* b = heap.deleteMin();
		HuffmanNode* dummy = new HuffmanNode();
		dummy->setRight(a);
		dummy->setLeft(b);
		dummy->setFrequency(a->getFrequency() + b->getFrequency());
		heap.insert(dummy);
	}
	return heap.deleteMin();
}

void readFile(int argc, char** argv, Heap &heap){
	if (argc != 2){
		cout << "An input file must be stated as the one and only command line parameter" << endl;
		exit(1);
	}

	ifstream file(argv[1]);
	if(!file.is_open()){
        cout << "Unable to open '" << argv[1] << "' for reading" << endl;
        exit(2);
	}

	unordered_map<char, unsigned int> charFrequencies; 

	char g;
	while(file.get(g)){
		if(!(g >= ' ' && g <= '~')){
			continue; //Not printable so dont care
		}
		unordered_map<char, unsigned int>::iterator element = charFrequencies.find(g);
		if(element == charFrequencies.end()){
			// New character, initalize
			charFrequencies[g] = 1;
		}
		else {
			// Repeated character, incriment
			unsigned int temp = element->second;
			temp++;
			charFrequencies.erase(element);
			charFrequencies[g] = temp;
		}
	}

	unordered_map<char, unsigned int>::iterator itr;
	for(itr = charFrequencies.begin(); itr != charFrequencies.end(); itr++){
		heap.insert(itr->first, itr->second);
	}

	HuffmanNode* root = buildHuffmanTree(heap);

	unordered_map<char, string> huffCodeLookup = generateLookup(root);

	unordered_map<char, string>::iterator itr2;
	for(itr2 = huffCodeLookup.begin(); itr2 != huffCodeLookup.end(); itr2++){
		if(itr2->first == ' '){
			cout << "space " << itr2->second << endl;
		}
		else{
			cout << itr2->first << " " << itr2->second << endl;
		}
	}


	cout << "----------------------------------------" << endl;
	file.clear();
	file.seekg(0);

	unsigned int numSymbols = 0;
	unsigned int numBitsCompressed = 0;
	while(file.get(g)){
		itr2 = huffCodeLookup.find(g);
		if(itr2 == huffCodeLookup.end()){
			continue;
		}
		cout << itr2->second << " ";
		if (g == ' '){
			cout << '\n';
		}
		numSymbols ++;
		numBitsCompressed += itr2->second.length();
	}
	cout << endl;
	cout << "----------------------------------------" << endl;

	cout << "There are a total of " << numSymbols << " symbols that are encoded." << endl;
	cout << "There are " << huffCodeLookup.size() << " distinct symbols used." << endl;
	cout << "There were " << numSymbols*8 << " bits in the original file." << endl;
	cout << "There were " << numBitsCompressed << " bits in the compressed file." << endl;
	double compressionRatio = ((numSymbols*8) / ((double) numBitsCompressed));
	cout << "This gives a compression ratio of " << compressionRatio << "." << endl;
	double costOfHuffman = 0.0;
	for(itr = charFrequencies.begin(); itr != charFrequencies.end(); itr++){
		for(itr2 = huffCodeLookup.begin(); itr2 != huffCodeLookup.end(); itr2++){
			if (!(itr->first == itr2->first)){
				continue;
			}
			int freq = itr->second;
			int leng = itr2->second.length();
			costOfHuffman += (((freq) / ((double) numSymbols)))*leng;
		}
	}
	cout << "The cost of the Huffman tree is " << costOfHuffman << " bits per character." << endl;
	file.close();
}

int main(int argc, char** argv){
	Heap heap = Heap(); 
	readFile(argc, argv, heap);
	return 0;
}