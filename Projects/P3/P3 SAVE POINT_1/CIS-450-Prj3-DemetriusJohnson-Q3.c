/*
 *Author: Demetrius Johnson
 * Project 3 - Question 3: Professor and Student Problem
 * Date: 3-17-22
 * Date Last Modified: 3-18-22
 * CIS-450, UM-Dearborn, with Dr. Jinhua Guo
 *
 * Notes from Professor Guo from the P3 discussion video and document:
 * *Requirement: use semaphores for synchronization:
(i) only one person is speaking at any one time, 
(ii) each student question is answered by the professor,
(iii) no student asks another question before the professor is done answering the previous one
(iv) No student can be done until their question is answered (professor considers it rude if they did)
Make sure that your simulation outputs information that clearly shows that your solution works.  
In particular, messages should be printed at the following times:
•	whenever a student is ready to ask a question
•	whenever a student starts asking a question
•	whenever a student is done asking a question
•	whenever a student receives the answer and leaves
•	whenever the professor wants to be asked a question
•	whenever the professor starts an answer
•	whenever the professor is finished with an answer

Also, insert a sched_yield() as the last line in 
each of the four functions and after each student is created; 
this will increase the chance of interleaving and
hence test your program more thoroughly.

 * Adapted from the bridge.txt and using condvar1.c file provided by Professor Guo
 */
#define _CRT_SECURE_NO_DEPRECATE //need this to use deprecated functions, such as fopen()
#include <stdbool.h> //need this to use true/false enum; functions just like c++ built-in bool system where false == 0 and true == !false
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     //The stdint.h header defines integer types, limits of specified width integer types, limits of other integer types and macros for integer constant expressions.
#include <inttypes.h>    //Include the standard header <inttypes.h> to define a type, several functions, and numerous macros for fine control over the conversion of integers.


//FUNCTION DECLARATIONS//
void* TheProfessor(void* args); //the two threads
void* OneStudent(void* args);

void AnswerStart();
void AnswerDone();
void QuestionStart();
void QuestionDone();
//FUNCTION DECLARATIONS//

//GOBAL VALUES
#define SHARED 0		//will set flag to 0 --> in sem_init function, it means the semaphore is shared for threads within the SAME process
sem_t speaking;			//global semaphores
int numStudents;       //command line argument will tell how many students



int main(int argc, char* argv[]) {


	//program protection; ensure main vector does not access nullptr/unallocated memory
	if (argc < 2) {
		printf("Usage: ProfStud <Number of Students>\n");
		printf(" e.g.: ./ProfStud 30 (prog name + 1 arguments)\n");
		exit(0);
	}
	numStudents = argv[1];

	//create the student threads and a default attribute that will be used for all threads
	pthread_t* studentArray;
	studentArray = (pthread_t*)malloc(numStudents * sizeof(pthread_t));

	//create professor thread for a single professor
	pthread_t professor;

	/*
   For portability (many threads can share the same attr variable),
   explicitly create threads in a joinable state;
   a pthread_attr_t object type must be initialzed to its default state before being customized;
   then all threads can all use this attr variable.
   */
	bool attr_FAIL;//use this to check attr function returns
	pthread_attr_t attr;
	attr_FAIL = pthread_attr_init(&attr);                                      //initialize a pthread attr variable to its default values
	if (attr_FAIL) {                                                          //ensure attribute was initialized successfully (return 0 == success)
		printf("***ATTR CREATION FAILED.***\n");
		//fprintf(filePtr, "***ATTR CREATION FAILED.***\n");
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //now that attr has been initialized we can edit its attributes; set the detach state of this attribute obj to 0 == joinable. 
	if (attr_FAIL) {                                                          //ensure attribute was modified successfully (return 0 == success)
		printf("**ATTR MODIFY FAILED.***\n");
		//fprintf(filePtr, "**ATTR MODIFY FAILED.***\n");
		exit(1);
	}

	//Now any threads initialized with this attr variable will inherit the attributes of the attr variable.
	bool pthread_FAIL; //use this bool to check pthread function returns

	 //now assign each thread a program to execute with the common attr thread attributes using pthread_create()
	 
	//assign professor thread
	pthread_create(&professor, &attr, TheProfessor, (void*));

	//assign student threads
	for (int i = 0; i < numStudents; i++) {
		
		pthread_FAIL = pthread_create(&CarArray[i], &attr, OneStudent, (void*)&args[i]); 

		//ensure all threads were created successfully (return 0 == success)
		if (pthread_FAIL) {
			printf("***THREAD CREATION FAILED***\n");
			//fprintf(filePtr, "***THREAD CREATION FAILED***\n");
			exit(1);
		}
	}

	//and tell main to wait for all child threads to exit before exiting using pthread_join()
	for (int j = 0; j < numCars; j++) {
		pthread_FAIL = pthread_join(CarArray[j], NULL);
		//ensure all threads join main thread successfully (return 0 == success)
		if (pthread_FAIL) {
			printf("***THREAD JOIN FAILED***\n");
			//fprintf(filePtr, "***THREAD JOIN FAILED***\n");
			exit(1);
		}
	}

	/* Clean up and exit */   //free dynamically allocated memory
	//fclose(filePtr); //close output file
	free(studentArray);
	pthread_attr_destroy(&attr);
	sem_close(&)
	pthread_exit(NULL); //main thread exits






}





//FUNCTION DEFINITIONS//
void* TheProfessor(void* args) {
	
	while (1) {
		AnswerStart();
		printf("The professor is answering the question...\n");
		AnswerDone();
	}
}

void* OneStudent(void* args) {

	QuestionStart();
	//printf("Student(%d) is asking a question...\n", studentnumber);
	QuestionDone();
}

	//professor functions:
void AnswerStart() {}
void AnswerDone() {}
	//student functions:
void QuestionStart() {}
void QuestionDone() {}
//FUNCTION DEFINITIONS//