/*
Robert Owens (rao7utn)
postfixCalculator.h
9/20/2020
*/

/*
Class declaration of postfixCalculator object
*/

#ifndef POSTFIXCALCULATOR_H
#define POSTFIXCALCULATOR_H

#include <iostream>
#include "Stack.h"

using namespace std;

class PostfixCalculator {
public:
	PostfixCalculator();
	~PostfixCalculator();

	void push(int e);
	int top();
	void pop();
	bool empty();

	void handleInput(string s);

private:
	Stack* stackPointer;

	void checkEmpty();
};

#endif