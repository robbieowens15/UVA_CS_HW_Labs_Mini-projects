/*
Robert Owens (rao7utn)
9/10/2020
List.cpp
*/

#include "List.h"
using namespace std;

List::List(){
	head = new ListNode();
	tail = new ListNode();
	head->next = tail;
	tail->previous = head;
	count = 0;
}

// Copy constructor
// Called when the code looks something like List list2 = list1;
// (In other words, it is called when you are trying to construct a **new** list from an existing one)
List::List(const List& source) {
    head=new ListNode();
    tail=new ListNode();
    head->next=tail;
    tail->previous=head;
    count=0;

    // Make a deep copy of the list
    ListItr iter(source.head->next);
    while (!iter.isPastEnd()) {
        insertAtTail(iter.retrieve());
        iter.moveForward();
    }
}

List::~List(){
	this->makeEmpty();
	delete head;
	delete tail;
}

// Copy assignment operator
// Called when the code looks something like list2 = list1;
// (In other words, it is called when you are trying to set an **existing** list equal to another one)
List& List::operator=(const List& source) {
    if (this == &source) {
        // The two are the same list; no need to do anything
        return *this;
    } else {
        // Clear out anything this list contained
        // before copying over the items from the other list
        makeEmpty();

        // Make a deep copy of the list
        ListItr iter(source.head->next);
        while (!iter.isPastEnd()) {
            insertAtTail(iter.retrieve());
            iter.moveForward();
        }
    }
    return *this;
}

bool List::isEmpty() const {
	if (head->next == tail){
		return true;
	}
	else{
		return false;
	}
}

void List::makeEmpty(){
	while (!this->isEmpty()){
		ListNode* toRemovePointer = head->next;
		ListNode* toFillPointer = toRemovePointer->next;
		toRemovePointer->next = NULL;
		toRemovePointer->previous = NULL;
		head->next = toFillPointer;
		toFillPointer->previous = head;

		delete toRemovePointer;
		count --;
	}
}

ListItr List::first(){
	return ListItr(head->next);
}

ListItr List::last(){
	return ListItr(tail->previous);
}

void List::insertAfter(int x, ListItr position){
	ListNode* addBehindThis = position.current;
	ListNode* addInfrontOfThis = addBehindThis->next;
	ListNode* newNodePointer = new ListNode(x);

	addBehindThis->next = newNodePointer;
	newNodePointer->previous = addBehindThis;
	addInfrontOfThis->previous = newNodePointer;
	newNodePointer->next = addInfrontOfThis;

	count ++;
}

void List::insertBefore(int x, ListItr position){
	ListNode* addInfrontOfThis = position.current;
	ListNode* addBehindThis = addInfrontOfThis->previous;
	ListNode* newNodePointer = new ListNode(x);

	addBehindThis->next = newNodePointer;
	newNodePointer->previous = addBehindThis;
	addInfrontOfThis->previous = newNodePointer;
	newNodePointer->next = addInfrontOfThis;

	count ++;
}

void List::insertAtTail(int x){
	ListNode* newNodePointer = new ListNode(x);
	ListNode* currentLastNodePointer = tail->previous;
	newNodePointer->next = tail;
	newNodePointer->previous = currentLastNodePointer;
	currentLastNodePointer->next = newNodePointer;
	tail->previous = newNodePointer;
	count++;
}

ListItr List::find(int x){
	ListItr navigator = this->first();
	while(!navigator.isPastEnd()){
		if (navigator.retrieve() == x){
			return navigator;
		}
		navigator.moveForward();
	}
	return navigator;
}

void List::remove(int x){
	ListItr navigator = this->find(x);
	if (navigator.isPastEnd()){
		return;
	} 
	else{
		ListNode* toRemovePointer = navigator.current;
		toRemovePointer->previous->next = toRemovePointer->next;
		toRemovePointer->next->previous = toRemovePointer->previous;
		toRemovePointer->next = NULL;
		toRemovePointer->previous = NULL;

		delete toRemovePointer;
		count --;
	}
}

int List::size() const {
	return count;
}

void printList(List& source, bool forward){
	if (forward){ 
		ListItr iterator= source.first();
		while(!iterator.isPastEnd()){
			cout << iterator.retrieve() << " ";
			iterator.moveForward();
		}
	}
	else{
		ListItr iterator= source.last();
		while(!iterator.isPastBeginning()){
			cout << iterator.retrieve() << " ";
			iterator.moveBackward();
		}
	}
	cout << endl;
}