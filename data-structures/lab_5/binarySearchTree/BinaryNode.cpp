/*
Robert Owens (rao7utn)
10/2/2020
BinaryNode.cpp
*/

#include "BinaryNode.h"
#include <string>
using namespace std;

BinaryNode::BinaryNode() {
    value = "?";
    left = NULL;
    right = NULL;
}

BinaryNode::BinaryNode(string x) {
    value = x;
    left = NULL;
    right = NULL;
}


BinaryNode::~BinaryNode() {
    delete left;
    delete right;
    left = NULL;
    right = NULL;
}