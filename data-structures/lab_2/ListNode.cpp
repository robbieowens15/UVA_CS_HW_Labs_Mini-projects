/*
Robert Owens (rao7utn)
9/10/2020
ListNode.cpp
*/

#include "ListNode.h"
using namespace std;

ListNode::ListNode(){
	value = 0;
	next = NULL;
	previous = NULL;
}

ListNode::ListNode(int x){
	value = x;
	ListNode* next = NULL;
	ListNode* previous = NULL;
}