/*
Robert Owens (rao7utn)
prelab4.cpp
9/24/2020
*/

#include <iostream>
#include <limits>
#include <math.h>

using namespace std;

void sizeOfTest(){
	cout << "Size of int: " << sizeof(int) << endl;
	cout << "Size of unsigned int: " << sizeof(unsigned int)  << endl;
	cout << "Size of float: " << sizeof(float) << endl;
	cout << "Size of double: " << sizeof(double) << endl;
	cout << "Size of char: " << sizeof(char) << endl;
	cout << "Size of bool: " << sizeof(bool) << endl;
	cout << "Size of int*: " << sizeof(int*) << endl;
	cout << "Size of char*: " << sizeof(char*) << endl;
	cout << "Size of double*: " << sizeof(double*) << endl;
}

void overflow(){
	unsigned int max = numeric_limits<unsigned int>::max();
	unsigned int max_p1 = max + 1;
	cout << max << " + 1 = " << max_p1 << endl;
}

void outputBinary(unsigned int x){
	string binaryRepresentation = "";
	for (int power = 31; power >= 0; power--){
		if (((power-3)%4 == 0) && power != 31){
			binaryRepresentation += " ";
		}
		unsigned int twosPlace = pow(2,power);
		if (x >= twosPlace){
			binaryRepresentation += "1";
			x -= twosPlace;
		}
		else{
			binaryRepresentation += "0";
		}
	}
	cout << binaryRepresentation << endl;
}

int main(){
	unsigned int x = 0;
	cin >> x;
	sizeOfTest();
	overflow();
	outputBinary(x);
	return 0;
}