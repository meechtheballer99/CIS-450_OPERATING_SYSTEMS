/*
 *Author: Demetrius Johnson   Date: 3-15-22
 * Understanding uintptr_t from inttypes.h and stdint.h; also understanding how to print using pointers
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     //The stdint.h header defines integer types, limits of specified width integer types, limits of other integer types and macros for integer constant expressions.
#include <inttypes.h>    //Include the standard header <inttypes.h> to define a type, several functions, and numerous macros for fine control over the conversion of integers.

//note on type casting: type casting casts the value pointed to by a variable as the type-cast passed to it.


void main() {

	int* ptr;			 // type = int*
	int intVal = 15;		//type = int
	ptr = &intVal;

	//printing the address of the ptr variable --> (&ptr == int**)

	printf("%p\n", &ptr);  //prints out address of the ptr pointer variable in hexadecimal
	printf("%p\n", &(void*)ptr); //prints the address of the ptr variable in hexadecimal
	printf("%i\n", &(void*)ptr); //prints the address of the ptr variable as an integer
	printf("%i\n", (void*)&ptr);//prints the address of the ptr variable as an integer

	//printing address of intVal, the value pointed to by ptr variable --> (&intVal == int*).

	printf("%p\n", ptr);  //prints address of int Val, the value stored by the ptr variable, in hexadecimal
	printf("%p\n", (uintptr_t)ptr); //prints out address pointed to by ptr in hexadecimal since we used %p, so uintptr_t had no effect.
	printf("%i\n", (uintptr_t)ptr); //prints out address pointed to by ptr as an integer; uintptr_t is an unsigned int with a storage capacity exactly the size needed to store an address as an integer.
	printf("%i\n",  (void*)ptr); //prints the same as printf("%i\n", (uintptr_t)ptr).
	printf("%i\n", ptr); //prints the same as printf("%i\n", (uintptr_t)ptr).


	//printing the value stored by intVal == 15.
	printf("%i\n", *ptr); 
	printf("%i\n", intVal);
	printf("%i\n", (uintptr_t)*ptr);
	printf("%p\n", (uintptr_t)*ptr); //prints 0000000F
	printf("%p\n", *ptr);            //prints 0000000F


}

