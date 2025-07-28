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
each of the four functions AND after each student is created;
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
void* TheProfessor(void* args); //the two thread types
void* OneStudent(void* args);

void AnswerStart(int question);
void AnswerDone(int question);
void QuestionStart(int student);
void QuestionDone(int student);
void WELCOME_MESSAGE(int argc, char* argv[]);
//END FUNCTION DECLARATIONS//

//GOBAL VALUES
#define SHARED 0		//will set flag to 0 --> in sem_init function, it means the semaphore is shared for threads within the SAME process
int numStudents;       //command line argument will tell how many students
int numQuestions;		//use this to determine when all students have asked a question and left; professor can now leave.
int* studentID;
int increase_interleave; //use this to create more interleaving 
//global semaphores
sem_t speaking_mutex;   //only one person allowed to speak at a time
sem_t question_cond;	//professor needs to wait for a question before answering
sem_t answered_cond;   //student can leave only after professor is done giving the answer to their question
FILE* filePtr; //use this to also send output to an output file

int main(int argc, char* argv[]) {

	//open a file and output results to that file
	filePtr = fopen("prof-stud_output.txt", "w"); //may need to use fopen_s instead of fopen() because fopen() is the obselete (depractated) version of the function; fopen_s() is the "safe" and more portable version of the function
	if (filePtr == NULL) {
		printf("***file not opened successfully; exiting program...***\n");
		exit(0);
	}

	//program protection; ensure main vector does not access nullptr/unallocated memory
	if (argc < 2) {
		printf("Usage: ProfStud <Number of Students>\n");
		printf(" e.g.: ./ProfStud 30 (prog name + 1 arguments)\n");
		exit(0);
	}
	numStudents = atoi(argv[1]); //get number of students in the class; this value will not change
	numQuestions = numStudents;  //get number of questions left to be asked in the class; this value WILL change

	//WELCOME MESSAGE:
	WELCOME_MESSAGE(argc, argv);

	//case: no students in class
	if (numStudents == 0) {

		printf("~No students in the class. Professor has gone to sleep.~\n");
		exit(0);
	}

	//initialize semaphores
	sem_init(&speaking_mutex, SHARED, 1); //mutex semaphore
	sem_init(&question_cond, SHARED, 0); //synchronization/conditional sempahore
	sem_init(&answered_cond, SHARED, 0); //synchronization/conditional conditional semaphore

	//create the student threads and a default attribute that will be used for all threads
	pthread_t* studentArray;
	studentArray = (pthread_t*)malloc(numStudents * sizeof(pthread_t));

	//create professor thread for a single professor
	pthread_t professor_thread;

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
		fprintf(filePtr, "***ATTR CREATION FAILED.***\n");
		exit(1);
	}
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //now that attr has been initialized we can edit its attributes; set the detach state of this attribute obj to 0 == joinable. 
	if (attr_FAIL) {                                                          //ensure attribute was modified successfully (return 0 == success)
		printf("**ATTR MODIFY FAILED.***\n");
		fprintf(filePtr, "**ATTR MODIFY FAILED.***\n");
		exit(1);
	}

	//Now any threads initialized with this attr variable will inherit the attributes of the attr variable.
	bool pthread_FAIL; //use this bool to check pthread function returns

	 //now assign each thread a program to execute with the common attr thread attributes using pthread_create():
	//assign professor thread some code to run (TheProfessor function)
	pthread_FAIL = pthread_create(&professor_thread, &attr, TheProfessor, NULL);
	if (pthread_FAIL) {
		printf("***THREAD CREATION FAILED***\n");
		fprintf(filePtr, "***THREAD CREATION FAILED***\n");
		exit(1);
	}

	//assign student threads some code to run (OneStudent function)
	studentID = (int*)malloc(numStudents * sizeof(int));			//use this to store student id and avoid race condition (multiple students assigned same ID)
	for (int i = 0; i < numStudents; i++) {
		studentID[i] = i;
		pthread_FAIL = pthread_create(&studentArray[i], &attr, OneStudent, (void*)&studentID[i]);
		//ensure all threads were created successfully (return 0 == success)
		if (pthread_FAIL) {
			printf("***THREAD CREATION FAILED***\n");
			fprintf(filePtr, "***THREAD CREATION FAILED***\n");
			exit(1);
		}
	}

	//and tell main to wait for all child threads to exit before exiting using pthread_join()
	pthread_FAIL = pthread_join(professor_thread, NULL);
	if (pthread_FAIL) {
		printf("***THREAD JOIN FAILED***\n");
		fprintf(filePtr, "***THREAD JOIN FAILED***\n");
		exit(1);
	}
	for (int j = 0; j < numStudents; j++) {
		pthread_FAIL = pthread_join(studentArray[j], NULL);
		//ensure all threads join main thread successfully (return 0 == success)
		if (pthread_FAIL) {
			printf("***THREAD JOIN FAILED***\n");
			fprintf(filePtr, "***THREAD JOIN FAILED***\n");
			exit(1);
		}
	}


	/* Clean up and exit */   //free dynamically allocated memory
	fclose(filePtr); //close output file
	free(studentArray);
	pthread_attr_destroy(&attr);
	pthread_exit(NULL); //main thread exits
	 //note, for unnamed sempahores (ex: sem_init(3) or sem_init(i) where i is an int), 
	 //you must call sem_destroy(int number) for all unnamed semaphores declared. 
	 //In this implementation we did not use any unnamed semaphores.
}





//FUNCTION DEFINITIONS//
void* TheProfessor(void* args) {


	int questionNum;
	while (numQuestions > 0) {
		questionNum = (numStudents - numQuestions);
		numQuestions--;
		AnswerStart(questionNum);
		printf("The Professor is ANSWERING question#%d.\n", questionNum);
		fprintf(filePtr, "The Professor is ANSWERING question#%d.\n", questionNum);
		AnswerDone(questionNum);
	}
	printf("~No more questions from students in the class. The Professor has gone to sleep.~\n");
	fprintf(filePtr, "~No more questions from students in the class. The Professor has gone to sleep.~\n");
	pthread_exit(NULL); //thread terminates once all questions have been answered
}

void* OneStudent(void* args) {

	if (rand() % 2)
		sched_yield(); //increase interleaving 

	int id = *((int*)args);
	QuestionStart(id);
	printf("Student_%d is ASKING a question.\n", id);
	fprintf(filePtr, "Student_%d is ASKING a question.\n", id);
	QuestionDone(id);

	pthread_exit(NULL); //thread terminates
}

//professor functions:
void AnswerStart(int question) {

	printf("The Professor is READY TO ANSWER question#%d.\n", question);
	fprintf(filePtr, "The Professor is READY TO ANSWER question#%d.\n", question);
	sem_wait(&question_cond); //wait for a student to ask a question
	sem_wait(&speaking_mutex); //one speaker at a time
	sched_yield(); //increase interleaving
}
void AnswerDone(int question) {

	printf("The Professor is DONE ANSWERING question#%d.\n", question);
	fprintf(filePtr, "The Professor is DONE ANSWERING question#%d.\n", question);
	sem_post(&answered_cond); //done answering question, signal student to leave
	sem_post(&speaking_mutex); //done speaking
	sched_yield(); //increase interleaving
}
//student functions:
void QuestionStart(int student) {

	printf("Student_%d is READY TO ASK a question.\n", student);
	fprintf(filePtr, "Student_%d is READY TO ASK a question.\n", student);
	sched_yield(); //increase interleaving
	sem_wait(&speaking_mutex);
	sched_yield(); //increase interleaving
}
void QuestionDone(int student) {

	printf("Student_%d is DONE ASKING a question.\n", student);
	fprintf(filePtr, "Student_%d is DONE ASKING a question.\n", student);
	sem_post(&speaking_mutex); //done speaking
	sem_post(&question_cond); //signal profess; a question has been asked
	sem_wait(&answered_cond); //wait until professor finishes answering your question
	printf("Student_%d received the answer and left.\n", student);
	fprintf(filePtr, "Student_%d received the answer and left.\n", student);
	sched_yield(); //increase interleaving
}

void WELCOME_MESSAGE(int argc, char* argv[]) {

	//WELCOME MESSAGING AND PROGRAM INFORMATION
	//console output:
	printf("WELCOME TO THE MULTITHREADED PROFESSOR-STUDENT QUESTION-ANSWER PROGRAM\nWritten BY DEMETRIUS JOHNSON\nFOR UM-DEARBORN CIS-450 WITH DR. JINHUA GUO\n");
	printf("NUMBER OF STUDENTS IN CLASS: %d.\n", atoi(argv[1]));
	printf("~Output also sent to prof-stud_output.txt file in directory of the executable.~\n\n");
	//file output:
	fprintf(filePtr, "WELCOME TO THE MULTITHREADED PROFESSOR-STUDENT QUESTION-ANSWER PROGRAM\nWritten BY DEMETRIUS JOHNSON\nFOR UM-DEARBORN CIS-450 WITH DR. JINHUA GUO\n");
	fprintf(filePtr, "NUMBER OF STUDENTS IN CLASS: %d.\n", atoi(argv[1]));
}
//END FUNCTION DEFINITIONS//