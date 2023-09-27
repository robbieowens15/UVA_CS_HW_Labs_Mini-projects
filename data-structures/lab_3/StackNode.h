/*
Robert Owens (rao7utn)
9/23/2020
StackNode.h
*/

#ifndef STACKNODE_H
#define STACKNODE_H

#include <iostream>

using namespace std;

class StackNode {
public:
    StackNode(int x);
    
private:
    int value;       
    StackNode* next;     

    friend class Stack;
};

#endif