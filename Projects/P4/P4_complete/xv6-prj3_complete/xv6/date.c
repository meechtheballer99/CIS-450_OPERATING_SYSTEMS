#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
	//NOW IN USER MODE IN A USER FUNCTION

  struct rtcdate r;
  
  //call the date system call function
  if (date(&r)) { //SWITCH TO KERNEL MODE BY CALLING SYSTEM CALL FUNCTION
    printf(2, "date failed\n");
    exit();
  }

       //RETURN TO USER MODE AND USER FUNCTION

  // print the time as a formatted string
  printf(1, "%d-%d-%d %d:%d:%d\n", 
           r.month, r.day, r.year, r.hour, r.minute, r.second);

          //END USER FUNCTION BY CALLING A
  exit(); //SYSTEM CALL TO KILL THE PROCESS; SWITCH TO KERNEL MODE AGAIN
}

