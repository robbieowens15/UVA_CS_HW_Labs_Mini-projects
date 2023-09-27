/*
Robert Owens (rao7utn)
postfixCalculator.cpp
9/20/2020
*/

/*
Implimentation of postfixCalculator object
*/

#include "postfixCalculator.h"
#include <set>
#include <string>
#include <iostream>

using namespace std;

PostfixCalculator::PostfixCalculator() {
	stackPointer = new Stack();
}

PostfixCalculator::~PostfixCalculator() {
	delete stackPointer;
}

void PostfixCalculator::push(int e) {
	stackPointer->push(e);
}

int PostfixCalculator::top() {
	this->checkEmpty();
	return stackPointer->top();
}

void PostfixCalculator::pop() {
	this->checkEmpty();
	stackPointer->pop();
}

bool PostfixCalculator::empty() {
	return stackPointer->empty();
}

void PostfixCalculator::checkEmpty(){
	if (this->empty()){
		cout << "Error: Stack Empty" << endl;
		exit(-1);
	}	
}

int compute(string operation, int first, int second);

void PostfixCalculator::handleInput(string s){
	string allOperators[] = {"+", "-", "*", "/", "~"};
	set<string> operators (allOperators, allOperators+5);

	if (operators.find(s) != operators.end()){
		// handle as operator
		if (s != "~"){
			int second = this->top();			
			this->pop();
			int first = this->top();
			this->pop();

			int temp = compute(s, first, second);

			this->push(temp);
		}
		else {
			// negator
			int temp = this->top();
			this->pop();
			temp *= -1;
			this->push(temp);
		}
	}
	else{
		// handle as number
		int temp = stoi(s);
		this->push(temp);
	}
}

int compute(string operation, int first, int second){
	if (operation == "+"){
		return first+second;
	}
	else if (operation == "-"){
		return first - second;
	}
	else if (operation == "*") {
		return first*second;
	}
	else {
		return first/second;
	}
}