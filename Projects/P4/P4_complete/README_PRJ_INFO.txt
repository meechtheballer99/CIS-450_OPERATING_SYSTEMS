*GO TO PAGE 140 (CHAPTER 9, FIGURE 9.3) IN THE OSTEP (Operating Systems, Three Easy Pieces) 
book to see implementation of the lottery ticket CPU scheduling.

*ticks = number of times a process was scheduled to run on the CPU; each tick is about 10ms.

USING THE PS FUNCTION:
-the below two processes will always be running in XV6
*proccessID 1 is the userinit() process; once it stops 
it doesn't really run anymore as it is the initial user process and the parent process for the xv6 OS,
 so ticks (times it was schedule to CPU) will remain mostly static.
*processID 2 is the share() process; this processes will continue to be ran throughout the life of XV6 running.
*upon first call to ps when you first boot XV6, processID 3 is the ps() function itself; 
thus when you call ps() again immediately after the first call, you will notice processID 3 is no longer in the system
 as it has terminated, and now processID 4 is the new call to ps(); etc...

*need wrapper function for system call: int yield() -- this is because we simply need to call the already existent
yield implementation that is a kernel function which thusly cannot be accessed via usermode; hence, we need
this simple wrapper system call so that we can switch from usermode to kernel mode and access the kernel-level
yield function.

*given schedular code is in the proc.c and proc.h files; it is round robin; thus we need tosimply change it from
round robin to lottery scheduling.

*startothers() function in main.c file for XV6 starts the other processers (if present/available; XV6 uses 2 by default); 
the lines above that function call are all ran on a single processor.

*in main.c for XV6, the first user process ran is called userinit().
-so, first we run OS in kernel mode, then first user process called is userinit().

*mpmain() in main.c of XV6 will call scheduler() so that the initial processor can start running processes.
-the startothers() function will also end up leading to a call to the scheduler() function so that all
subsequent CPUs that start running with the initial CPU will also run the schedular() function to run processes.
-so both CPUs in XV6 will end up calling the scheduler() function to run user processes; 
after calling the userinit() function, they both will get to the scheduler.



*our job is to change the scheduler() function in the proc.c file from a round robin scheme to lottery.

IN SCHEDULER FUNCTION FROM PROC.C FILE:
-mycpu() function returns which CPU is accessing the scheduler function; returns an int (0 = cpu0, 1 = cpu1).
-scheduler is an infinite loop so that any processes that will enter the system can then start to be ran 
by the CPU via the scheduler() function.
-ptable = process table.
-NPROC == 64; in XV6, you can only create a maximum of 64 processes.
-ptable struct has a lock variable that must be acquired before updating the fixed proc[NPROC] struct array.
-in the proc[NPROC] struct array, it has all information about each process.
-



