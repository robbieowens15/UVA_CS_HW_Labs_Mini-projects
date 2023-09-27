#include <iostream>
#include <string>
#include "Person.h"
using namespace std;

bool createPerson();
bool deletePerson();
void printAllPeople();

int main() {
	while(true){
		//Menu
		cout << "1. Make person\n"
		"2. Delete person\n"
		"3. Print all people\n"
		"0. Quit\n"
		"Selection: ";
		//Input
		int selection = -1;
		cin >> selection;
		cout << endl;

		if (selection == 1){
			cout << "Success (bool)?: " <<createPerson() << endl;
		}
		else if (selection == 2){
			cout << "Success (bool)?: " <<deletePerson() << endl;
		}
		else if (selection == 3){
			printAllPeople();
		}
		else if (selection == 0){
			return 0;
		}
		else{
			cout << "Invalid Input" << endl;
		}
	}
	return 0;
}

bool createPerson(){
	return true;
}

bool deletePerson(){
	return false;
}

void printAllPeople(){

}