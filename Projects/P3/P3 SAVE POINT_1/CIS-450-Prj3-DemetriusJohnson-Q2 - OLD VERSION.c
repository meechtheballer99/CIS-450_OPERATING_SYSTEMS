/*
 *Author: Demetrius Johnson
 * Project 3 - Question 2: The One Lane Bridge Problem
 * Date: 3-12-22
 * Date Last Modified: 3-14-22
 * CIS-450, UM-Dearborn, with Dr. Jinhua Guo
 *
 * Notes from Professor Guo from the P3 discussion video:
 Also, use condvar1.c from sampleP1.tar to see an example of implementing 
 this monitor-style logic using pthread mutex and condition variables.
 Also, when assinging a car thread a direction, 
 use a random number generator (use seed time based on system time).
 *
 *Also, insert a sched_yield() as the last line in each of the functions; 
 this will increase the chance of interleaving
 and hence test your program more thoroughly.
 Go to 19:27 in the P3 video for more details.
 *
 * Adapted from the bridge.txt and using condvar1.c file provided by Professor Guo
 */
#include <stdbool.h> //need this to use true/false enum; functions just like c++ built-in bool system where false == 0 and true == !false
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     //The stdint.h header defines integer types, limits of specified width integer types, limits of other integer types and macros for integer constant expressions.
#include <inttypes.h>    //Include the standard header <inttypes.h> to define a type, several functions, and numerous macros for fine control over the conversion of integers.


enum direction {north, south}; //0==north, 1==south
int RandomAssigned_dir;

//use this struct to point to the arguments of the "monitor" functions
struct arg_struct { int direc; int car_id; }; 


 //One Lane Bridge Monitor Solution

//monitor Bridge{
pthread_mutex_t mutex_BridgeAccess;
pthread_cond_t safe;
int currentDirec;  //currentDirec and currentNumber are shared (critical section) variables and need to be protected via mutex locks
int currentNumber;

/*
   Lock mutex and wait for signal.  Note that the pthread_cond_wait routine
   will automatically and atomically unlock mutex while it waits.
*/
void ArriveBridge(int direc, int id) {

    pthread_mutex_lock(&mutex_BridgeAccess);
    direc == north ? printf("car_%d-(N) ARRIVES at bridge\n", id) : printf("car_%d-(S) ARRIVES at bridge\n", id);
    while (!is_Safe(direc))
        pthread_cond_wait(&safe, &mutex_BridgeAccess); //unlocks when sleep; reacquires lock when it wakes up and runs
    currentNumber++;
    currentDirec = direc; //bridge is now safe; make the current travel direction this car's direction 
    pthread_mutex_unlock(&mutex_BridgeAccess);
    sched_yield(); //all sched_yield are placed in this program to increase the chance of interleaving and allow for more thorough program testing
}

//no locks necessary here since car is crossing the bridge and will not change any shared variables.
void CrossBridge(int direc, int id) {
    direc == north ? 
        printf("car_%d-(N) CROSSING bridge; direction: NORTH; #cars on bridge: %d\n", id, currentNumber) :
        printf("car_%d-(S) CROSSING bridge; direction: SOUTH; #cars on bridge: %d\n", id, currentNumber);
    sched_yield(); 
}

void ExitBridge(int direc, int id) {

    pthread_mutex_lock(&mutex_BridgeAccess); //acquire lock
    currentNumber--;
    direc == north ? printf("car_%d-(N) EXITS from bridge\n", id) : printf("car_%d-(S) EXITS from bridge\n", id);
    pthread_cond_broadcast(&safe); //wake up all threads (tell all cars the bridge is safe to cross)
    pthread_mutex_unlock(&mutex_BridgeAccess); //release lock
   sched_yield(); 
}

int is_Safe(int direc) {
    if (currentNumber == 0)
        return true;    // always safe when bridge is empty
    else if ((currentNumber < 3) && (currentDirec == direc))
        return true;    // room for us to follow others in direc
    else
        return false;   // bridge is full or has oncoming traffic.

   
}

//} end Bridge monitor



//the car/vehicle thread
void* OneVehicle(void* args) {
    int myDirection = ((struct arg_struct*)args)->direc;
    int carID = ((struct arg_struct*)args)->car_id;
    ArriveBridge(myDirection, carID); //mutex locks and conditions required here
    CrossBridge(myDirection, carID); //this function is sandwiched and protected by necessary mutex and conditions placed in the arrive and exit functions; no mutex or conditions required here.
    ExitBridge(myDirection, carID); //mutex locks required here
    pthread_exit(NULL); //thread exits
}


int main(int argc, char* argv[]) {

     /* Initialize mutex and condition variable objects */ //notice, for attr objects, and for mutex and cond objects, they must be initialized before being used and customized (if desired)
    pthread_mutex_init(&mutex_BridgeAccess, NULL);
    pthread_cond_init (&safe, NULL);


    int numCars = 50; //default number of car threads to create for this program

    if (argc == 2) { //otherwise, assign number of car threads to create based on user input from Command Line Argument parameter
    
        numCars = atoi(argv[1]); //0 == program name, 1==first arugment
    }

    //create the car threads and a default attribute that will be used for all car threads
    pthread_t* CarArray;
    CarArray = (pthread_t*)malloc(numCars * sizeof(pthread_t));
   
   /*
  For portability (many threads can share the same attr variable),
  explicitly create threads in a joinable state; 
  a pthread_attr_t object type must be initialzed to its default state before being customized; 
  then all threads can all use this attr variable.
  */
    pthread_attr_t attr;
    pthread_attr_init(&attr);                                      //initialize a pthread attr variable to its default values
   // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //now that attr has been initialized we can edit its attributes; set the detach state of this attribute variable to 0 == joinable. 
                                                                    //Now any threads initialized with this attr variable will inherit the attributes of the attr variable.
    //initialize the argument struct
    struct arg_struct* args = (struct arg_struct*)malloc(sizeof(struct arg_struct));

     //now assign each car thread a program to execute with the common attr thread attributes using pthread_create()
    for (int i = 0; i < numCars; i++) {
        bool thread_CreateSuccess;
        //assign random direction; if randomn number generated leaves no remainder after divide by 2 (RandomAssigned_dir % 2 == 0), then it is an even number; odd otherwise.
        RandomAssigned_dir = rand();
        RandomAssigned_dir = (RandomAssigned_dir % 2 == 0) ? north : south;
        args->direc = RandomAssigned_dir;
        args->car_id = i;
        thread_CreateSuccess = pthread_create(&CarArray[i], &attr, OneVehicle, (void*)args); //the last two paraemeters in this pthread_create function are paremeters passed to the OneVehicle() function
        
        //ensure all threads were created successfully
        if (!thread_CreateSuccess) {
            printf("***THREAD CREATION UNSUCCESSFULL***\n");
            exit(1);
        }
    }

    //and tell main to wait for all child threads to exit before exiting using pthread_join()
    for (int j = 0; j < numCars; j++) {
        pthread_join(CarArray[j], NULL);
    }

    /* Clean up and exit */   //free dynamically allocated memory
    free(CarArray); 
    free(args);
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&mutex_BridgeAccess);
    pthread_cond_destroy(&safe);
    pthread_exit(NULL); //main thread exits
}