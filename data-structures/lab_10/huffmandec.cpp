/*
Robert Owens (rao7utn)
11/9/2020
huffmandec.cpp
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include <string>

#include "huffmanNode.h"

using namespace std;

string decodeMessage(HuffmanNode* huffmanTreeRoot, const string decodedMessage){
    HuffmanNode* curNode = huffmanTreeRoot;
    string message = "";
    for(int i = 0; i < decodedMessage.length(); i++){
        if (!(decodedMessage.at(i) == '0' || decodedMessage.at(i) == '1')){
            continue;
        }

        if(decodedMessage.at(i) == '0'){
            curNode = curNode->getLeft();
        }
        else{
            curNode = curNode->getRight();
        }
       
        if (curNode->isLeaf()){
            message += curNode->getValue();
            curNode = huffmanTreeRoot;
        }
    }
    return message;
}

void createTree(HuffmanNode*& curNode, const char character, const string prefix, unsigned int i){
    if(curNode == NULL){
        curNode = new HuffmanNode();
    }

    //base case
    if(i == prefix.length()){
        curNode->setValue(character);
        return;
    }

    if(prefix.at(i) == '0'){
        createTree(curNode->getLeft(), character, prefix, i+1);
    }
    else{
        createTree(curNode->getRight(), character, prefix, i+1);
    }
}

HuffmanNode* generateHuffTree(const unordered_map<char, string> table){
    HuffmanNode* root = new HuffmanNode();
    unordered_map<char, string>::iterator itr;
    for(auto& itr: table){
        createTree(root, itr.first, itr.second, 0);
    }
    return root;
}

void readFile(int argc, char** argv){
    // verify the correct number of parameters
    if (argc != 2) {
        cout << "Must supply the input file name as the only parameter" << endl;
        exit(1);
    }

    // attempt to open the supplied file
    // must be opened in binary mode as otherwise trailing whitespace is discarded
    ifstream file(argv[1], ifstream::binary);
    // report any problems opening the file and then exit
    if (!file.is_open()) {
        cout << "Unable to open file '" << argv[1] << "'." << endl;
        exit(2);
    }

    unordered_map<char, string> huffCodeLookup = unordered_map<char, string>();

    // read in the first section of the file: the prefix codes
    while (true) {
        string character, prefix;
        // read in the first token on the line
        file >> character;

        // did we hit the separator?
        if (character[0] == '-' && character.length() > 1) {
            break;
        }

        // check for space
        if (character == "space") {
            character = " ";
        }

        // read in the prefix code
        file >> prefix;
        // do something with the prefix code
        //cout << "character '" << character << "' has prefix code '" << prefix << "'" << endl;
        huffCodeLookup[character[0]] = prefix;
    }

    HuffmanNode* huffmanTreeRoot = generateHuffTree(huffCodeLookup);

    // read in the second section of the file: the encoded message
    stringstream sstm;
    while (true) {
        string bits;
        // read in the next set of 1's and 0's
        file >> bits;
        // check for the separator
        if (bits[0] == '-') {
            break;
        }
        // add it to the stringstream
        sstm << bits;
    }

    string allbits = sstm.str();
    // at this point, all the bits are in the 'allbits' string
    //cout << "All the bits: " << allbits << endl;
    // close the file before exiting
    file.close();

    string message = decodeMessage(huffmanTreeRoot, allbits);
    cout << message << endl;
}

// main(): we want to use parameters
int main (int argc, char** argv) {
    readFile(argc, argv);
    return 0;
}