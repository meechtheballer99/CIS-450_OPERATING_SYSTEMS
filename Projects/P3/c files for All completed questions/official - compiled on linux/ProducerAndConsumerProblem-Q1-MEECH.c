/*
 *Author: Demetrius Johnson
 * Project 3 - Question 1: Producer and Consumer Problem
 * Date 3-10-22
 * Date Last Modified: 3-18-22
 * CIS-450, UM-Dearborn, with Dr. Jinhua Guo
 *
 * A complete example of simple producer/consumer program. The Producer
 * and Consumer functions are executed as independent threads.
 * They share access to a MULTIPLE buffers, dataBuf (number of buffers determined by command line parameter).
 * Each producer deposits a sequence of integers from 1 to numIters (also a command line parameter) into a buffer.
 * A Consumer fetches a value stored in a buffer and adds them. Two semaphores, empty and full, are used to
 * ensure that producers and consumers alternate access to the buffers, and one semaphore, mutex, is used to
 * ensure mutual exclusion to prevent a deadlock and incorrect results (overlapping reads/writes, reading an old buffer value, etc)
 * when modifying the data buffer array.
 *
 * Adapted from the boundedBuffer.c program that Professor Guo gave us to reference.
 */
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>

#define SHARED 0 //will set flag to 0 --> in sem_init function, it means the semaphore is shared for threads within the SAME process

void* Producer(void*); // the two threads
void* Consumer(void*);

sem_t empty, full, mutex;       //global semaphores
int* dataBuf;                   //shared buffers, size = n (numBuf). Use front and rear variables to point to a given buffer
int numBuf;
int bufPos_front = 0;
int bufPos_rear = 0;
int numIters;                  //number of iterations each producer will execute to create SUM[i=0 to i = n](i + i+1 + ... + i+n).

// main() -- read command line and create threads
int main(int argc, char* argv[]) {

    //create 3 producer threads and 3 consumer threads data types (this will assign each thread an id)
    pthread_t pid[3];
    pthread_t cid[3];

    //program protection; ensure vector does not access nullptr/unallocated memory
    if (argc < 3) {
        printf("Usage: boundedBuffer <Number of Iterations> <Number of buffers>\n");
        printf(" e.g.: ./boundedBuffer 100 10 (prog name + 2 arguments)\n");
        exit(0);
    }
    numIters = atoi(argv[1]); //get num iterations each producer will execute from command line argument
    numBuf = atoi(argv[2]); //get buffer size from command line argument, translate it to an int
    dataBuf = (int*)malloc(numBuf * sizeof(int)); //allocate buffer size

    //full and empty act as synchronization and condition variables, mutex acts as a mutual exclusion critical section lock variable
    sem_init(&empty, SHARED, numBuf);    // sem empty = numBuf specified by command line argument
    sem_init(&full, SHARED, 0);     //sem full = 0 
    sem_init(&mutex, SHARED, 1);    //sem mutex = 1, indicating it is a binary semaphote = mutex variable.

    //assign all threads an attribute (each thread will be created with the given stack/function code).
    for (int i = 0; i < 3; i++) {
        pthread_create(&pid[i], NULL, Producer, NULL);
        pthread_create(&cid[i], NULL, Consumer, NULL);
    }

    //tell main to wait for all child threads to exit before exiting.
    for (int j = 0; j < 3; j++) {
        pthread_join(pid[j], NULL);
        pthread_join(cid[j], NULL);
    }

    free(dataBuf); //free dynamically allocated memory
    //note, for unnamed sempahores (ex: sem_init(3) or sem_init(i) where i is an int), you must call sem_destroy(int number) for all unnamed semaphores declared. In this implementation we did not use any unnamed semaphores.
    pthread_exit(0); //exit (terminate) the current thread, in this case: the main thread
}

// deposit 1, ..., numIters into the dataBuf[] buffers
void* Producer(void* arg) {
    int produced;

    for (produced = 0; produced < numIters; produced++) {
        sem_wait(&empty); //condition: there must be at least 1 empty buffer available in order to try to write to a buffer in the buffer array.
        sem_wait(&mutex); //mutual exclusion in the critical section: only one thread at a time allowed to either write or read (produce or consumer); this prevents deadlock and incorrect/overlapping buffer read/write.
        dataBuf[bufPos_rear] = produced; //write to buffer
        bufPos_rear = (bufPos_rear + 1) % numBuf;  //move to next buffer location; need to incorporate wrap around for the buffer array, so we use % operator
        sem_post(&mutex); //allow another into critical section to access and modify the data buffer array.
        sem_post(&full); //if there is a Consumer waiting for at least 1 buffer to be filled, notify them.
    }
    pthread_exit(NULL);//terminate thread
}

//fetch numIters items from the buffer and sum them
void* Consumer(void* arg) {
    int total = 0;
    int consumed;

    for (consumed = 0; consumed < numIters; consumed++) {
        sem_wait(&full); //condition: cannot try to consume unless consumer is notified that a data buffer has been filled
        sem_wait(&mutex); //mutual exclusion in the critical section: only one thread at a time allowed to either write or read (produce or consumer); this prevents deadlock, etc...
        total = total + dataBuf[bufPos_front]; //read/consume data buffer
        bufPos_front = (bufPos_front + 1) % numBuf;  //need to incorporate wrap around for the buffer array, so we use % operator
        sem_post(&mutex); //unlock CS
        sem_post(&empty); //notify any producers that a buffer is empty
    }

    printf("the total is %d\n", total);
    pthread_exit(NULL);//terminate thread
}
