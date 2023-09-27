#ifndef PERSON_H
#define PERSON_H
#include <string>

using namespace std;

class Person {
	public:
		Person(string name = "", int id=0);

		int getID() const;
		string getName() const;
		int getPopulation() const;
		void toString() const;
		void setName(string newName);

	private:
		const int id;
		string name;
		static int population;
};

#endif