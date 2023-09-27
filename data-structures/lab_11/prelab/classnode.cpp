/*
Robert Owens (rao7utn)
11/16/2020
classnode.cpp
*/

#include <iostream>
#include <stddef.h> //for NULL
#include "classnode.h"

ClassNode::ClassNode() {
    name = "";
    numInEdges = 0;
    numOutEdges = 0;
    outEdges = vector<ClassNode*>();
}

ClassNode::ClassNode(const string& n) {
    name = n;
    numInEdges = 0;
    numOutEdges = 0;
    outEdges = vector<ClassNode*>();
}
ClassNode::~ClassNode(){

}

void ClassNode::print(){
    cout << name + "[" << numInEdges << ", " << numOutEdges << "]" << endl;
    printOutEdges();
}

void ClassNode::printOutEdges(){
    for(int i = 0; i < outEdges.size(); i++){
        cout << "\t" + outEdges.at(i)->getName();
    }
    cout << endl;
}

string ClassNode::getName(){
    return name;
}

unsigned int ClassNode::getNumInEdges(){
    return numInEdges;
}

unsigned int ClassNode::getNumOutEdges(){
    return numOutEdges;
}

vector<ClassNode*> ClassNode::getOutEdges(){
    return outEdges;
}

void ClassNode::setName(string n){
    name = n;
}

void ClassNode::incNumInEdges(){
    numInEdges++;
}

void ClassNode::decNumInEdges(){
    numInEdges--;
}

void ClassNode::setNumInEdges(unsigned int i){
    numInEdges = i;
}

void ClassNode::incNumOutEdges(){
    numOutEdges++;
}

void ClassNode::decNumOutEdges(){
    numOutEdges--;
}

void ClassNode::setNumOutEdges(unsigned int i){
    numOutEdges = i;
}

void ClassNode::addOutEdge(ClassNode* n){
    outEdges.push_back(n);
    bubbleSort();
}

void ClassNode::bubbleSort(){
    for(int i = 0; i < outEdges.size(); i++){
        for(int j = 1; j < outEdges.size() - i; j++){
            if(outEdges.at(j-1)->getName() > outEdges.at(j)->getName()){
                ClassNode* temp = outEdges.at(j-1);
                outEdges.at(j-1) = outEdges.at(j);
                outEdges.at(j) = temp;
            }
        }
    }
}