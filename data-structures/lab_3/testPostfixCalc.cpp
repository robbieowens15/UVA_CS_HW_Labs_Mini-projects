/*
Robert Owens (rao7utn)
testPostfixCalc.cpp
9/20/2020
*/

/*
Hard-coded expression and evaluates that expression
*/

#include "postfixCalculator.h"
#include <iostream>

using namespace std;

int main() {
	PostfixCalculator calculator = PostfixCalculator();

	string token;
	while (cin >> token) {
	    calculator.handleInput(token);
	}
	cout << "" << calculator.top() << endl;
	return 0;
}