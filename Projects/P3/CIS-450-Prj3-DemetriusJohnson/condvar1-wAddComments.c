/*
* FILE: condvar1.c
* DESCRIPTION:
*   Example code for using Pthreads condition variables.  The main thread
*   creates three threads.  Two of those threads increment a "count" variable,
*   while the third thread watches the value of "count".  When "count"
*   reaches a predefined limit, the waiting thread is signaled by one of the
*   incrementing threads.
*
* SOURCE: Adapted from example code in "Pthreads Programming", B. Nichols
*   et al. O'Reilly and Associates.
* LAST REVISED: 02/16/2014  J. Guo
*/
/*Here are the default attributes of the attr object from pthread_attr:

Thread attributes :
Detach state = PTHREAD_CREATE_JOINABLE
Scope = PTHREAD_SCOPE_SYSTEM
Inherit scheduler = PTHREAD_INHERIT_SCHED
Scheduling policy = SCHED_OTHER
Scheduling priority = 0
Guard size = 4096 bytes
Stack address = 0x40196000
Stack size = 0x201000 bytes
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     //The stdint.h header defines integer types, limits of specified width integer types, limits of other integer types and macros for integer constant expressions.
#include <inttypes.h>    //Include the standard header <inttypes.h> to define a type, several functions, and numerous macros for fine control over the conversion of integers.

#define NUM_THREADS  3
#define TCOUNT 10
#define COUNT_LIMIT 12

int count = 0;
pthread_mutex_t count_mutex;
pthread_cond_t count_threshold_cv;

//the two threads with critical sections in each thread
void* watch_count(void* idp);
void* inc_count(void* idp);

int main(int argc, char *argv[])
{
    int i, rc;
    pthread_t threads[3];
    pthread_attr_t attr;

    /* Initialize mutex and condition variable objects */ //notice, for attr objects, and for mutex and cond objects, they must be initialized before being used and customized (if desired)
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init (&count_threshold_cv, NULL);

    /*
    For portability (many threads can share the same attr variable), explicitly create threads in a joinable state; a pthread_attr_t object type must be initialzed to its default state before being customized; then all threads can all use this attr variable.
    */
    pthread_attr_init(&attr); //initialize a pthread attr variable to its default values
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //now that attr has been initialized we can edit its attributes; set the detach state of this attribute variable to 0 == joinable. Now any threads initialized with this attr variable will inherit the attributes of the attr variable.

    //3 threads initialized/created using the attr attribute object
    pthread_create(&threads[0], &attr, inc_count, (void *) 0);
    pthread_create(&threads[1], &attr, inc_count, (void *) 1);
    pthread_create(&threads[2], &attr, watch_count, (void *) 2);

    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
    pthread_exit (NULL); //main thread exits
}



void* inc_count(void* idp)
{
    int j, i;
    double result = 0.0;
    unsigned int  my_id;

    //note: uintptr_t is an unsigned integer type exactly the size needed to store an address as an integer; 
    //thus via typecasting, the value stored by idp is casted as an unsigned int of the sizeof  uintptr_t.
    my_id = (uintptr_t)idp;

    for (i = 0; i < TCOUNT; i++) {
        pthread_mutex_lock(&count_mutex);
        count++;

        printf("inc_count(): thread %d, count = %d\n",
            my_id, count);

        /*
        Check the value of count and signal waiting thread when condition is
        reached.  Note that this occurs while mutex is locked.
        */
        if (count == COUNT_LIMIT) {
            pthread_cond_signal(&count_threshold_cv);
            printf("inc_count(): thread %d, count = %d  Threshold reached.\n",
                my_id, count);
        }
        pthread_mutex_unlock(&count_mutex);

        /* Do some work so threads can alternate on mutex lock */
        for (j = 0; j < 1000; j++)
            result = result + (double)rand();
        sched_yield(); //thread gives up CPU and goes to back of the line in the ready queue
    }
    pthread_exit(NULL); //thread exits
}

void* watch_count(void* idp)
{
    unsigned int my_id;

    my_id = (intptr_t)idp;

    printf("Starting watch_count(): thread %d\n", my_id);

    /*
    Lock mutex and wait for signal.  Note that the pthread_cond_wait routine
    will automatically and atomically unlock mutex while it waits.
    Also, note that if COUNT_LIMIT is reached before this routine is run by
    the waiting thread, the loop will be skipped to prevent pthread_cond_wait
    from never returning.
    */
    pthread_mutex_lock(&count_mutex); //need a mutex lock to check value of the global count variable and make sure to go to sleep if necessary, that way when the signal is sent to wake up, this thread won't miss it and go to sleep indefintely.
    while (count < COUNT_LIMIT) {
        pthread_cond_wait(&count_threshold_cv, &count_mutex); //wait, and unlock mutex while waiting; when returns (wakes up after receiving signal), it will automaticall and atomically acquire the mutex lock again.
        printf("watch_count(): thread %d Condition signal received.\n", my_id);
    }
    pthread_mutex_unlock(&count_mutex); //release lock
    pthread_exit(NULL); //thread exits
}