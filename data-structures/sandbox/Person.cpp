#include "Person.h"
#include <iostream>
#include <string>
using namespace std;

int Person::population = 1;

Person::Person(string initialName, int initalID) :
	id(initalID){
		name = initialName;
		population++;	
	}

string Person::getName() const {
	return name;
}

void Person::setName(string newName) {
	name = newName;
}

int Person::getID() const {
	return id;
}

int Person::getPopulation() const {
	return population;
}

void Person::toString() const {
	cout << "id("+ to_string(id)+") -> "+ name << endl;
}