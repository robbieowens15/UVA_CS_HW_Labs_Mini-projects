/*
Robert Owens (rao7utn)
10/12/2020
hashtable.h
*/

#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>

using namespace std;

class Hashtable {
public:
	Hashtable(unsigned int size);
	~Hashtable();

	bool find(string s);
	void insert(string s);
private:
	string* table;
	unsigned int tableSize;
	unsigned int* powersLookupTable;

	unsigned int hashFunction1(string s);
	unsigned int hashFunction2(string s);

	bool checkprime(unsigned int p);
	int getNextPrime(unsigned int n);
};

#endif