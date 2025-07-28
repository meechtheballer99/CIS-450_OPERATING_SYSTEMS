***the output for this program is in prof-stud_output.txt

*for this program, I added extra sched_yield() functions in order to increase interleaving. 
I even used the rand() function to determine randomnly if a thread would yield.

*I notice since MAIN function has highest priority, the sched_yield() had no effect, so as stated above, I used
measures to mitigate this and successfully increase interleaving.

*the flow of the program is as follows (if you were to stack all of the primary functions and semaphores):

1)prof is WAITINGfor a question, then WAITING to speak if no one else is.
2)student is ready to ask a question, and is WAITING to speak.
3)when the professor answers a question, he SIGNALS that he is done talking, and SIGNALS that a student can leave
4) a student asks a question when no one is talking,  then SIGNALS they are done talking, then WAITS for professor to answer question.
-this all happens until every student has asked a question, and the professor has answered all of them.
-note: 
	Questions are answered in order; no student is skipped, 
	I labeled the question# just for ease of following the program,
 	but it could correspond to any student ID as to who asked the question.
	whoever asked the question first was the first student to get their question answered.
	It also made seeing the interleaving of the program easier, and allowed me to make adjustments to try and improve it.