/*
Robbie Owens (rao7utn)
bitCounter.cpp
9/30/2020
*/

#include <string>
#include <string.h>
#include <iostream>
#include <map>
#include <math.h> 

using namespace std;

int numOnes(int number){
	if (number == 0){
		return 0;
	}
	else if (number % 2 == 0){
		return numOnes(number/2);
	}
	else {
		return 1 + numOnes(number/2);
	}
}

map<char,int> generateLookUpTableToNumber(){
	map<char, int> t;
	int counter = 0;
	for (int i = 0x30; i <= 0x5A; i ++){
		t[i] = counter;
		counter ++;
		if (i == 0x39){
			i = 0x40;
		}
	}
	return t;
}

map<char,int> generateLookUpTableToChar(){
	map<char, int> t;
	int counter = 0;
	for (int i = 0x30; i <= 0x5A; i ++){
		t[counter] = i;
		counter ++;
		if (i == 0x39){
			i = 0x40;
		}
	}
	return t;
}

int convertFromXBaseToDecimal(string toConvert, int base){
	int numberOfBits = toConvert.length();
	char bitArray[numberOfBits + 1];
	strcpy(bitArray, toConvert.c_str());

	map<char,int> lookUpTable = generateLookUpTableToNumber();
	int decimalNumber = 0;
	int power = numberOfBits - 1;

	for (int i = 0; i < numberOfBits; i++){
		decimalNumber += lookUpTable[bitArray[i]]*pow(base, power);
		power --;
	}
	return decimalNumber;
}

string baseConverter(string toConvert, int fromBase, int toBase){
	int asDecimal = convertFromXBaseToDecimal(toConvert, fromBase);

	int highestPosition = (int) (log(asDecimal) / log(toBase)) +1;
	int power = highestPosition - 1;
	string convertedNumber = "";
	map<char, int> lookUpTable = generateLookUpTableToChar();

	for(int i = 0; i < highestPosition; i++){
		int placeValue = asDecimal/pow(toBase, power);
		char digit = lookUpTable[placeValue];
		convertedNumber += digit;

		asDecimal -= (placeValue*pow(toBase, power));
		power --;
	}

	return convertedNumber;
}

int main (int argc, char *argv[]){

	if (argc <= 4){
		cout << "Error: 3 command-line arguments needed for this script" << endl;
		exit(-1);
	}

	/*Argument to numOnes*/
	int someBaseTenNumber = stoi((argv[1]));
	//All letters are UPPERCASE
	string numToConvert(argv[2]);
	//Bases assumed between 1-36
	int orignialBase = stoi((argv[3]));
	int toConvertBase = stoi((argv[4]));

	cout << someBaseTenNumber << " has " << numOnes(someBaseTenNumber) << " bit(s)" << endl;
	cout << numToConvert << " (base " << orignialBase << ") = " 
		+ baseConverter(numToConvert, orignialBase, toConvertBase) + " (base " << toConvertBase << ")" << endl;

	return 0;
}