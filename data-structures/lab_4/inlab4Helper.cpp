/*
Robert Owens (rao7utn)
inlab4Helper.cpp
9/24/2020
*/

#include <iostream>
#include <limits>
#include <iomanip>
using namespace std;

int main(){
	cout << "\tMax Values" << endl;
	cout << "Max int: " << numeric_limits<int>::max() << endl;
	cout << "Max unsigned int: " << numeric_limits<unsigned int>::max() << endl;
	cout << "Max float: " << numeric_limits<float>::max() << endl;
	cout << "Max double: " << numeric_limits<double>::max() << endl;
	cout << "Max char: " << numeric_limits<char>::max() << endl;
	cout << "Max bool: " << numeric_limits<bool>::max() << endl;

	int integer = 1;
	unsigned int unsignedInterger = 1;
	float floatingPoint = 1.0;
	double doubleFloatingPoint = 1.0;
	char character = '1';
	bool boolean = true;

	int* intPointer = NULL;
	char* charPointer = NULL;
	double* doublePointer = NULL;

	char maxChar = numeric_limits<char>::max();
	bool maxbool = numeric_limits<bool>::max();
	double maxDouble = numeric_limits<double>::max();

	return 0;
}