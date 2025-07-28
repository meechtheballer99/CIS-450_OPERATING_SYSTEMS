

// CIS-450-Prj3-DemetriusJohnson.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/*Author: Demetrius Johnson; adapted C++ version of the thread/semaphore program
 * boundedBuffer.cpp
 *
 * A complete example of simple producer/consumer program. The Producer
 * and Consumer functions are executed as independent threads.  They
 * share access to a single buffer, data.  The producer deposits a sequence
 * of integers from 1 to numIters into the buffer.  The Consumer fetches
 * these values and adds them.  Two semaphores,empty and full are used to
 * ensure that the producer and consumer alternate access to the buffer.
 *
 * SOURCE: adapted from example code in "Multithreaded, Parallel, and
 *         Distributed Programming" by Gregory R. Andrews.
 */
#include <thread>
#include <semaphore>
#include <iostream>





void Producer(); // the two threads
void Consumer();

std::binary_semaphore empty(1), full(0);       //global semaphores // sem empty = 1 //sem full = 0
int data;                // shared buffer, size = 1
int numIters;

// main() -- read command line and create threads
int main(int argc, char* argv[]) {
    std::thread pid, cid;
    if (argc < 2) {
        std::cout << "Usage: boundedBuffer <Number of Iterations>\n";
        std::cout << " e.g.: ./boundedBuffer 100 \n";
        exit(0);
    }
    numIters = atoi(argv[1]);

    pid = std::thread(Producer);
    cid = std::thread(Consumer);

    //threads will be allowed to finish before main exits.
    pid.join();
    cid.join();

    //pthread_exit(0);
}

// deposit 1, ..., numIters into the data buffer
void Producer() {
    int produced;

    for (produced = 0; produced < numIters; produced++) {
        empty.acquire();
        data = produced;
        full.release();
    }
}

//fetch numIters items from the buffer and sum them
void Consumer() {
    int total = 0;
    int consumed;

    for (consumed = 0; consumed < numIters; consumed++) {
        full.acquire();
        total = total + data;
        empty.release();
    }

    std::cout << "the total is " << total << std::endl;
}

