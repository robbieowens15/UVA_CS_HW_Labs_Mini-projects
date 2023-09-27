/*
Robert Owens (rao7utn)
10/2/2020
BinaryNode.h
*/

#ifndef BINARYNODE_H
#define BINARYNODE_H
#include <string>
using namespace std;

class BinaryNode {
    BinaryNode();
    BinaryNode(string x);
    ~BinaryNode();

    string value;
    BinaryNode* left;
    BinaryNode* right;

    friend class BinarySearchTree;
};

#endif