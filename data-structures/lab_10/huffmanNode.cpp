/*
Robert Owens (rao7utn)
11/8/2020
huffmanNode.cpp
*/

#include <iostream>
#include <stddef.h> //for NULL
#include "huffmanNode.h"

HuffmanNode::HuffmanNode() {
    value = '-';
    frequency = 0;
    left = NULL;
    right = NULL;
}

HuffmanNode::HuffmanNode(const char& val, const unsigned int& freq) {
    value = val;
    frequency = freq;
    left = NULL;
    right = NULL;
}

HuffmanNode::~HuffmanNode(){

}

void HuffmanNode::print(){
	cout << "( '" << value << "', " << frequency << ")";
}

bool HuffmanNode::isLeaf() const{
	return left == NULL && right == NULL;
}

HuffmanNode*& HuffmanNode::getRight(){
	return right;
}

HuffmanNode*& HuffmanNode::getLeft(){
	return left;
}

char HuffmanNode::getValue() const{
	return value;
}

unsigned int HuffmanNode::getFrequency() const{
	return frequency;
}

void HuffmanNode::setRight(HuffmanNode* n){
	right = n;
}
void HuffmanNode::setLeft(HuffmanNode* n){
	left = n;
}
void HuffmanNode::setFrequency(unsigned int i){
	frequency = i;
}
void HuffmanNode::setValue(char c){
	value = c;
}