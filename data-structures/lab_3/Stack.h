/*
Robert Owens (rao7utn)
9/23/2020
Stack.h
*/

#ifndef STACK_H
#define STACK_H

#include <iostream>
#include "StackNode.h"

using namespace std;

class Stack {
public:

    Stack();

    ~Stack();

    bool empty() const;
    void push(int e);
    void pop ();
    int top() const;

private:
    StackNode* head;          
};

#endif