#include "types.h"
#include "x86.h"
#include "pstat.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//send the user program the current run time clock (RTC)
//this function will act as a wrapper function of the cmostime() function
int
sys_date(void) //written by Demetrius E Johnson UM-Dearborn CIS450 w/ Prof Guo
{
  struct rtcdate *r; //use this to acquire the reference
	               //passed to date(&r) system call in date.c

  if(argptr(0, (void*)&r, sizeof(*r)) < 0)
    return -1;
  cmostime(r);//cmostime takes a reference of type struct rtcdate
 		//now r from the date function has been modified 
  return 0;
}

int
sys_settickets(void)//written by Demetrius E Johnson UM-Dearborn CIS450 w/Prof Guo
{
 int numTickets;
 
 //get the value passed into the user system call function (from user.h)
 if(argint(0, &numTickets) < 0)
    return -1;

 return settickets(numTickets); //call the settickets function in proc.c
}


int
sys_getpinfo(void) //written by Demetrius E Johnson UM-Dearborn CIS450 w/Prof Guo
{
 struct pstat *pstats;

 if(argptr(0, (void*)&pstats, sizeof(*pstats)) < 0)
    return -1;//if no valid arguments, i.e. NULL pointer, return with error (-1)
 
 return getpinfo(pstats);//call getpinfo function in proc.c
}


int
sys_yield(void) //written by Demetrius E Johnson UM-Dearborn CIS450 w/Prof Guo
{

 yield();
 return 0;

}
