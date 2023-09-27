#include <stdio.h>
#include <stdlib.h>
int main() {
	int length = 0;
	printf("Enter how many values to input: ");
	scanf("%d", &length);

	int* intArray = (int*) malloc(sizeof(int) * length);
    if (intArray == NULL) {
        // memory allocation failed; handle the error somehow
        printf("Memory Allocation Failed");
        return 1;
    }

    for(int i = 0; i < length; i++){
    	int temp = 0;
    	printf("Enter value %d: ", i);
		scanf("%d", &temp);
		intArray[i] = temp;
    }

    for(int i = length -1; i >= 0; i--){
    	printf("%d\n", intArray[i]);
    }

    // free up that array
    free(intArray);
}