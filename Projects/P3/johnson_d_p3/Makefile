CFLAGS= -g
LDFLAGS= -lpthread -lrt
LDFLAGS1= -lpthread
CC=g++

all: ProducerAndConsumerProblem-Q1-MEECH OneLaneBridgeProblem-Q2-MEECH ProfessorAndStudentProblem-Q3-MEECH

# To make an executable
ProducerAndConsumerProblem-Q1-MEECH: ProducerAndConsumerProblem-Q1-MEECH.o 
	$(CC) -o ProducerAndConsumerProblem-Q1-MEECH ProducerAndConsumerProblem-Q1-MEECH.o $(LDFLAGS)

OneLaneBridgeProblem-Q2-MEECH: OneLaneBridgeProblem-Q2-MEECH.o 
	$(CC) -o OneLaneBridgeProblem-Q2-MEECH OneLaneBridgeProblem-Q2-MEECH.o $(LDFLAGS)

 ProfessorAndStudentProblem-Q3-MEECH: ProfessorAndStudentProblem-Q3-MEECH.o 
	$(CC) -o ProfessorAndStudentProblem-Q3-MEECH ProfessorAndStudentProblem-Q3-MEECH.o $(LDFLAGS)

# To make an object from source
.c.o:
	$(CC) $(CFLAGS) -c $*.c

# clean out the dross
clean:
	-rm ProducerAndConsumerProblem-Q1-MEECH OneLaneBridgeProblem-Q2-MEECH ProfessorAndStudentProblem-Q3-MEECH *.o

