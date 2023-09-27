/*
Robert Owens (rao7utn)
10/2/2020
TestCalc.cpp
*/

// TreeCalc.cpp:  CS 2150 Tree Calculator method implementations

#include "TreeCalc.h"
#include <iostream>
#include <set>
using namespace std;

// Constructor
TreeCalc::TreeCalc() {
	expressionStack = stack<TreeNode*>();
}

// Destructor - frees memory
TreeCalc::~TreeCalc() {
	cleanTree(expressionStack.top());
}

// Deletes tree/frees memory
void TreeCalc::cleanTree(TreeNode* tree) {
	if (tree == NULL){
		return;
	}
	cleanTree(tree->left);
	cleanTree(tree->right);
	delete tree;
}

// Gets data from user
// DO NOT MODIFY
void TreeCalc::readInput() {
    string response;
    cout << "Enter elements one by one in postfix notation" << endl
         << "Any non-numeric or non-operator character,"
         << " e.g. #, will terminate input" << endl;
    cout << "Enter first element: ";
    cin >> response;
    //while input is legal
    while (isdigit(response[0]) || response[0] == '/' || response[0] == '*'
            || response[0] == '-' || response[0] == '+') {
        insert(response);
        cout << "Enter next element: ";
        cin >> response;
    }
}

bool isOperator(string s){
	string allOperators[] = {"+", "-", "*", "/"};
	set<string> operators (allOperators, allOperators+4);

	return operators.find(s) != operators.end();
}

// Puts value in tree stack
void TreeCalc::insert(const string& val) {
    // insert a value into the tree
    TreeNode* newNode = new TreeNode(val);
    if (isOperator(val)){
    	newNode->right = expressionStack.top();
    	expressionStack.pop();
    	newNode->left = expressionStack.top();
    	expressionStack.pop();
    	expressionStack.push(newNode);
    }
    else {
    	expressionStack.push(newNode);
    }
}

// Prints data in prefix form
void TreeCalc::printPrefix(TreeNode* tree) const {
    // print the tree in prefix format
    if (tree == NULL){
    	return;
    }
    cout << tree->value + " ";
    printPrefix(tree->left);
    printPrefix(tree->right);
}

// Prints data in infix form
void TreeCalc::printInfix(TreeNode* tree) const {
    // print tree in infix format with appropriate parentheses
    if (tree == NULL){
    	return;
    }
    else if (isOperator(tree->value)){
    	cout << "(";
    }
    printInfix(tree->left);
    if (isOperator(tree->value)){
    	cout << " " + tree->value + " ";
    }
	else{
		cout << tree->value;
	}
    
    printInfix(tree->right);
    if (isOperator(tree->value)){
    	cout << ")";
    }
}

//Prints data in postfix form
void TreeCalc::printPostfix(TreeNode* tree) const {
    // print the tree in postfix form
    if (tree == NULL){
    	return;
    }
    printPostfix(tree->left);
    printPostfix(tree->right);
    cout << tree->value + " ";
}

// Prints tree in all 3 (post, in, pre) forms
// DO NOT MODIFY
void TreeCalc::printOutput() const {
    if (expressionStack.size() != 0 && expressionStack.top() != NULL) {
        TreeNode* tree = expressionStack.top();
        cout << "Expression tree in postfix expression: ";
        printPostfix(tree);
        cout << endl;

        cout << "Expression tree in infix expression: ";
        printInfix(tree);
        cout << endl;

        cout << "Expression tree in prefix expression: ";
        printPrefix(tree);
        cout << endl;
    } else {
        cout << "Size is 0." << endl;
    }
}

// Evaluates tree, returns value
// private calculate() method
int TreeCalc::calculate(TreeNode* tree) const {
    // Traverse the tree and calculates the result
    if (tree == NULL){
    	return 0;
    }
    else if (isOperator(tree->value)){
    	int first = calculate(tree->left);
    	int second = calculate(tree->right);
    	if (tree->value == "+"){
    		return first + second;
    	}
    	else if (tree->value == "-"){
    		return first - second;
    	}
    	else if (tree->value == "*"){
    		return first*second;
    	}
    	else{
    		return first/second;
    	}
    }
    else {
    	return stoi(tree->value);
    }
}

//Calls calculate, sets the stack back to a blank stack
// public calculate() method. Hides private data from user
int TreeCalc::calculate() {
    // call private calculate method here
    if (expressionStack.empty()){
    	cout << "\nError: Invalid Postfix Calculator Tree, No elements" << endl;
    	exit(-1);
	}
	else{
		return calculate(expressionStack.top());
	}
}