/*
Robert Owens (rao7utn)
linkedlist.c
11/3/2020
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define PUSH_FRONT 	0
#define PUSH_BACK 	1
#define POP_FRONT 	2
#define POP_BACK 	3
#define PRINT_LIST 	4
#define HELP 		5
#define QUIT 		6
#define INVALID 	7

#define BUFFER_SIZE 20

void printOptions();
int readInput(int* val);
int isNumber(const char* token);
int isEmpty(const char* input);

int main(){

	// TODO: INSTANTIATE YOUR LINKED LIST HERE.
	struct list_item {
    	struct list_item* prev;
    	struct list_item* next;
    	int value;
	} list_item_t;

	struct list {
    	struct list_item* head;
    	struct list_item* tail;
    	unsigned int length;
    	int (*compare)(const void* key, const void* with);
    	void (*datum_delete)(void*);
	} list_t;

	struct list* linkedList = malloc(sizeof(list_t));
	linkedList->head = NULL;
	linkedList->tail = NULL;
	//----END----

	int option, val;
	printOptions();

	do {
		option = readInput(&val);
		struct list_item* node;

		switch(option){
			case PUSH_FRONT:				// push onto front of list
				// TODO: Insert code to push val onto front of linked list here.
				node = malloc(sizeof(list_item_t));
				node->value = val;
				if (linkedList->head == NULL){
					linkedList->head = node;
					linkedList->tail = node;
					node->next = NULL;
					node->prev = NULL;
				}
				else {
					struct list_item* oldHead = linkedList->head;

					node->next = oldHead;
					node->prev = NULL;
					oldHead->prev = node;
					linkedList->head = node;
				}
				linkedList->length ++;
				//----END----
				break;
			case PUSH_BACK: 				// push onto back of list
				// TODO: Insert code to push val onto back of linked list here.
				node = malloc(sizeof(list_item_t));
				node->value = val;
				if(linkedList->tail == NULL){
					linkedList->head = node;
					linkedList->tail = node;
					node->next = NULL;
					node->prev = NULL;
				}
				else{
					struct list_item* oldTail = linkedList->tail;

					node->prev = oldTail;
					node->next = NULL;
					oldTail->next = node;
					linkedList->tail = node;
				}
				linkedList->length ++;
				//----END----
				break;
			case POP_FRONT: 				// remove from front of list
				// TODO: Insert code to remove from front of linked list here.
				// If list is empty, do nothing.
				if (linkedList->head == NULL){
					break;
				}
				else{
					struct list_item* toDelete = linkedList->head;
					if (toDelete->next == NULL){
						free(toDelete);
						linkedList->head = NULL;
						linkedList->tail = NULL;
					}
					else{
					struct list_item* newHead = toDelete->next;

					linkedList->head = newHead;
					newHead->prev = NULL;
					free(toDelete);
					}
					linkedList->length --;
				}

				//----END----
				break;
			case POP_BACK:					// remove from back of list
				// TODO: Insert code to remove from back of linked list here.
				// If list is empty, do nothing.
				if (linkedList->tail == NULL){
					break;
				}
				else{
					struct list_item* toDelete = linkedList->tail;
					if (toDelete->prev == NULL){
						free(toDelete);
						linkedList->head = NULL;
						linkedList->tail = NULL;
					}
					else{
						struct list_item* newTail = toDelete->prev;

						linkedList->tail = newTail;
						newTail->next = NULL;
						free(toDelete);						
					}
					linkedList->length --;
				}

				//----END----
				break;
			case PRINT_LIST:				// print list
				// TODO: Insert code to print list forwards here.
				// Simply print each element separated by a space as shown below:
				// Elements: 1 2 3 4 5 
				printf("Elements: ");
				struct list_item* curNode = linkedList->head;
				while(curNode != NULL){
					struct list_item* nextNode = curNode->next;
					printf("%d ", curNode->value);
					curNode = nextNode;
				}
				printf("\n");

				//----END----
				break;
			case HELP: 						// print menu
				printOptions();
				break;
			case QUIT: 						// quit
				break;
			case INVALID: 					// bad input
				fprintf(stderr,"Invalid command or operand, please input a valid command or help to see the list again.\n");
				break;
		}
	} while(option != QUIT);

	// TODO: free any memory used by your linked list here

	//Loop through and dealocate memory
	struct list_item* toDelete = linkedList->head;
	while(toDelete != NULL){
		struct list_item* nextNode = toDelete->next;
		free(toDelete);
		toDelete = nextNode;
	}

	free(linkedList);

	//----END----

	return 0;
}



//---------------------------------------
//NOTE: EVERYTHING BELOW THIS LINE SHOULD NOT BE TOUCHED. IT HANDLES INPUT AND SUCH FOR YOU
//---------------------------------------

int readInput(int* val){
	char input[BUFFER_SIZE]="";
	while(isEmpty(input)){
		printf("Enter command: ");
		fgets(input, BUFFER_SIZE, stdin);
		input[strcspn(input, "\n")] = 0;
	}

	char* token = strtok(input, " ");
	
	if ((strcmp(token, "af")) == 0){
		token = strtok(NULL, " ");
		if (isNumber(token)){
			*val = atoi(token);
			return PUSH_FRONT;
		} else return INVALID;
	} else if ((strcmp(token, "ab")) == 0){
		token = strtok(NULL, " ");
		if (isNumber(token)){
			*val = atoi(token);
			return PUSH_BACK;
		} else return INVALID;
	} else if ((strcmp(token, "rf")) == 0){
	 	return POP_FRONT;
	} else if ((strcmp(token, "rb")) == 0){
	 	return POP_BACK;
	} else if ((strcmp(token, "p")) == 0){
	 	return PRINT_LIST;
	} else if((strcmp(token, "help")) == 0){
	 	return HELP;
	} else if((strcmp(token, "-1")) == 0){
	 	return QUIT;
	} else {
		return INVALID;
	}
}

void printOptions(){
	printf("----------------------------------------------------\n");
	printf("This test harness operates with one linked list.\n");
	printf("Use any of the following options to manipulate it:\n");
	printf("\t* af <num> --- add integer to front\n");
	printf("\t* ab <num> --- add integer to back\n");
	printf("\t* rf       --- remove front element\n");
	printf("\t* rb       --- remove back element\n");
	printf("\t* p        --- print list forward\n");
	printf("\t* help     --- print off this list\n");
	printf("\t* -1       --- exit harness\n\n");
}

int isNumber(const char* token){
	int length, i = 0;
	if (token == NULL)	return 0;
	if(token[0] == '-') 	i = 1;
	length = strlen(token);
	for (; i < length; i++){
	    if (!isdigit(token[i]))
	    	return 0;
	}

	return 1;
}

int isEmpty(const char* input){
	while (*input != '\0'){
		if (!isspace((unsigned char)*input))
			return 0;
		input++;
	}

	return 1;
}