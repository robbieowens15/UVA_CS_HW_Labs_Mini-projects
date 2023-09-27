/*
Robert Owens (rao7utn)
9/10/2020
ListItr.cpp
*/

#include "ListItr.h"
using namespace std;

ListItr::ListItr(){
	current = NULL;
}

ListItr::ListItr(ListNode* theNode){
	current = theNode;
}

bool ListItr::isPastEnd() const {
	if (current->next == NULL){
		return true;
	}
	else{
		return false;
	}
}

bool ListItr::isPastBeginning() const {
	if (current->previous == NULL){
		return true;
	}
	else{
		return false;
	}
}

void ListItr::moveForward(){
	if (!this->isPastEnd()){
		current = current->next;
	}
}

void ListItr::moveBackward(){
	if (!this->isPastBeginning()){
		current = current->previous;
	}
}

int ListItr::retrieve() const {
	return current->value;
}