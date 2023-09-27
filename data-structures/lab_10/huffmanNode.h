/*
Robert Owens (rao7utn)
11/8/2020
huffmanNode.h
*/


#ifndef HUFFMANNODE_H
#define HUFFMANNODE_H

using namespace std;

class HuffmanNode {
public:
    HuffmanNode();                  // Default Constructor
    HuffmanNode(const char& val, const unsigned int& freq); 	// Constructor
    ~HuffmanNode();

    bool isLeaf() const;
    void print();

    HuffmanNode*& getRight();
    HuffmanNode*& getLeft();
    char getValue() const;
    unsigned int getFrequency() const;

    void setRight(HuffmanNode* n);
    void setLeft(HuffmanNode* n);
    void setFrequency(unsigned int i);
    void setValue(char c);

private:
    char value;
    unsigned int frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    friend class Heap;
};

#endif