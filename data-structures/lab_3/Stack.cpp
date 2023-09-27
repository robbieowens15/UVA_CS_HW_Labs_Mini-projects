/*
Robert Owens (rao7utn)
9/23/2020
Stack.cpp
*/

#include "Stack.h"
using namespace std;

Stack::Stack(){
	this->head = NULL;
}

Stack::~Stack(){
	while(!this->empty()){
		this->pop();
	}
}

bool Stack::empty() const {
	return this->head == NULL;
}

void Stack::push(int i) {
	StackNode* newNode = new StackNode(i);
	newNode->next = this->head;
	this->head = newNode;
}

void Stack::pop() {
	StackNode* toPop = this->head;
	this->head = toPop->next;
	delete toPop;
}

int Stack::top() const {
	return this->head->value;
}