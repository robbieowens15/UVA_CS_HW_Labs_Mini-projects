/*
Robert Owens (rao7utn)
10/2/2020
AVLNode.h
*/

#ifndef AVLNODE_H
#define AVLNODE_H
#include <string>
using namespace std;

class AVLNode {
    AVLNode();
    AVLNode(string x);
    ~AVLNode();

    string value;
    AVLNode* left;
    AVLNode* right;
    int height;

    friend class AVLTree;
};

#endif