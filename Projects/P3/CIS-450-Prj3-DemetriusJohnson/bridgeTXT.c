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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>     //The stdint.h header defines integer types, limits of specified width integer types, limits of other integer types and macros for integer constant expressions.
#include <inttypes.h>    //Include the standard header <inttypes.h> to define a type, several functions, and numerous macros for fine control over the conversion of integers.


enum direction {north, south}; //0==north, 1==south

 //One Lane Bridge Monitor Solution

//monitor Bridge{
Condition safe;

int currentDirec;
int currentNumber;

void ArriveBridge(int direc) {
    while (!isSafe(direc))
        safe.Wait();
    currentNumber++;
    currentDirec = direc;
}

void CrossBridge(int direc) {
    //crossing the bridge;
}

void ExitBridge(Bridge::Direction direc) {
    currentNumber--;
    safe.Broadcast();
}

bool isSafe(int direc) {
    if (currentNumber == 0)
        return TRUE;    // always safe when bridge is empty
    else if ((currentNumber < 3) && (currentDirec == direc))
        return TRUE;    // room for us to follow others in direc
    else
        return FALSE;   // bridge is full or has oncoming traffic.
}

//}



//the car/vehicle thread
void OneVehicle(int direc) {
    ArriveBridge(direc);
    CrossBridge(direc);
    ExitBridge(direc);
}


