#include <stdio.h>
#include <stdlib.h>
int main() {
    // for (int i = 0; i < 3; i++) {
    //     printf("hello world!\n");
    // }

 //    int x = 3;
	// printf("this is an int: %d\n", x);

	// double pi = 3.13159;

 //    printf("pi = %.0f\n", pi);         /* output: pi = 3    */
	// printf("pi = '%+10.4f'\n", pi);     pi = '   +3.1416' 
	// printf("Hello from %s, which begins at %p!\n",  __PRETTY_FUNCTION__, main);  /* Hello from main, which begins at 0x4004f4! */

	/*
	How to read in files
	*/
	// int open(const char *pathname, int flags);
	// int open(const char *pathname, int flags, mode_t mode);

	/*
	Note: but C does not support function overloading! While the open()
	manual page displays the two prototypes above, the actual signature of open() is:
	 */
	//int open(const char *pathname, int flags, ...);

	/*
	How to safely read input from a keyboard
	*/
	// int age;
	// char grade;
	// char school[3];

	// printf("AGE: ");
	// scanf("%d", &age);    /* Converts input to an integer and stores it in age */

	// printf("GRADE: ");
	// scanf("%c", &grade);  /* Converts input to a letter grade (probably 'A') and stores it in grade */

	
	// Note:
	// The third example above almost certainly overflows the buffer. 
	// scanf() will copy input into school until it sees the next whitespace character. 
	// If the input is “The University of Virginia”, it will save “The”, 
	// and overflow the buffer by one byte (due to the fact that all C-style strings 
	// have a zero byte that terminates the string). If the input is “UVA”, 
	// it will save “UVA”, but still overflow the buffer. Using the field width flag %2s 
	// can solve this buffer overflow problem, but then we will only save “Th” or “UV”. 
	// In order to convert whitespace, you must use the more complex conversion. 
	// It’s more common to use fgets() for this type of input.
	

	// printf("SCHOOL: ");
	// scanf("%s", school);  /* Converts input to a string and stores it in school */

	/*
	Dynamic Memory management
	*/

	// malloc() [new]
	// free() [delete]

	// dynamically allocate an array of ints
    // int* p = (int*) malloc(sizeof(int) * 5);
    // if (p == NULL) {
    //     // memory allocation failed; handle the error somehow
    //     return 1;
    // }

    // // Initialize p[1] to 10. Everything else is still uninitialized.
    // // Trying to access any other index without first initializing it is undefined behavior!
    // p[1] = 10;
    // printf("%d\n", p[1]);

    // // free up that array
    // free(p);

    /*
	Derived Types
    */

    // Structures (similar to objects)

 //    struct name {
	//     type1 member1;
	//     type2 member2;
	//     /* ... */
	//     typen membern; //optional but can be helpful
	// };

	//Example
	// struct list_item {
	//     struct list_item* prev;
	//     struct list_item* next;
	//     void* datum;
	// } list_item_t; // what you want to call your struct goes after the closing brace;

	/*
	Note:
	With the typedef, we can now refer to our struct as simply list_item_t. 
	Therefore, variable declarations become list_item_t my_item. 
	Note that the struct list_item* prev inside of the struct declaration 
	cannot be replaced with list_item_t* prev, as the typedef hasn’t been 
	finished by that point! We don’t know the name of the typedef until after the closing brace.
	*/

	/* Union */

	struct example_struct {
	    int type;
	    union {
	        int i;
	        float f;
	        double d;
	    };
	};

	struct example_struct s;
	s.type = 1;

	switch (s.type) {
	case 0:
	    printf("%d\n", s.i);
	    break;
	case 1:
	    printf("%+2.3f\n", s.f);
	    break;
	case 2:
	    scanf("%lf", &s.d);
	    break;
	}

	return 0;
}