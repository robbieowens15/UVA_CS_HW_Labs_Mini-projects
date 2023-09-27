/*
Robert Owens (rao7utn)
10/12/2020
hashtable.cpp
*/

#include "Hashtable.h"
#include <math.h>
#include <string>
#include <iostream>

using namespace std;

#define LOADFACTOR 0.75

Hashtable::Hashtable(unsigned int size){
	tableSize = getNextPrime(size/LOADFACTOR);
	table = new string[tableSize];

	powersLookupTable = new unsigned int[22];
	for (int i = 0; i < 22; i++){
		powersLookupTable[i] = pow(37, i);
	}

}

Hashtable::~Hashtable(){

}

bool Hashtable::find(string s){
	unsigned int index = hashFunction1(s);
	if (table[index] != s){
		index = hashFunction2(s);
	}
	do {
		if(table[index] == s){
			return true;
		}
		else if (table[index] == ""){
			return false;
		}
		else{
			index++;
			if (index == tableSize){
				index = 0;
			}
		}
	} while(true);
}

void Hashtable::insert(string s){
	unsigned int index = hashFunction1(s);
	if (table[index] != ""){
		index = hashFunction2(s);
	}
	while(table[index] != ""){
		index++;
		if (index == tableSize){
			index = 0;
		}
	}
	table[index] = s;
}

//Inital Hash
unsigned int Hashtable::hashFunction1(string s){
	unsigned int sum = 0;
	for(int i = 0; i < s.length(); i++){
		sum += s.at(i)*powersLookupTable[i];
	}
	return sum%tableSize;
}

//Rehash
unsigned int Hashtable::hashFunction2(string s){
	unsigned int sum = 0;
	int counter = 0;
	for(int i = s.length()-1; i >= 0; i--){
		sum += s.at(i)*powersLookupTable[counter];
		counter ++;
	}
	return sum%tableSize;
}

// yes, there are much faster -- and much better -- ways to check if a
// number is prime (see the Sieve of Eratosthenes, for one example),
// but this code will work correctly, it is fairly straightforward,
// and it is fast enough for our purposes in the hash lab
bool Hashtable::checkprime(unsigned int p) {
    if ( p <= 1 ) // 0 and 1 are not primes; the are both special cases
        return false;
    if ( p == 2 ) // 2 is prime
        return true;
    if ( p % 2 == 0 ) // even numbers other than 2 are not prime
        return false;
    for ( int i = 3; i*i <= p; i += 2 ) // only go up to the sqrt of p
        if ( p % i == 0 )
            return false;
    return true;
}

int Hashtable::getNextPrime (unsigned int n) {
    while ( !checkprime(++n) );
    return n; // all your primes are belong to us
}