#include <iostream>
#include <stdio.h>
#include <string.h>
#include "mpi.h" // message passing interface
using namespace std;

// New compile and run commands for MPI!
// mpicxx -o blah file.cpp
	//-o tells the name of the output file
// mpirun -q -np 32 blah
	//-q mode = quiet mode, will complain less
	//-np = number of process you will run it with

int main (int argc, char * argv[]) {

	int my_rank;			// my CPU number for this process (process ID number)
	int p;					// number of CPUs (processors) that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number (ID number for a given message in a chain)
	char evenMessage[100];	// message for even ring
	char oddMessage[100];	// message for odd ring
	MPI_Status status;		// return status for receive
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
    //Creating the messages and "sent lists"
	char * evenRing = evenMessage;					//Even ring = first ring, sends to even processors
	sprintf(evenRing, "Even Ring: ");
	sprintf(evenRing + strlen(evenRing), "%d ", my_rank);
	
	char * oddRing = oddMessage; 					//Odd ring = second ring, sends to odd processors
	sprintf(oddRing, "Odd Ring: ");
	sprintf(oddRing + strlen(oddRing), "%d ", my_rank);		
	
	//Begin the sending/receiving process
	if(my_rank == 0)			//Start at process 0, go to next even processor, return to process 0 (even ring)
	{
		//cout << "test test is this thing (" << my_rank << ") on" << endl; 		//Tester print statement
		MPI_Send(evenRing, strlen(evenRing) + 1, MPI_CHAR, 2, tag, MPI_COMM_WORLD); 
			//Sending to 2 b/c that is the next even processor
		 if(p%2 == 0)
            MPI_Recv(evenRing, 100, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD, &status);
        else
            MPI_Recv(evenRing, 100, MPI_CHAR, p - (p%2), tag, MPI_COMM_WORLD, &status);
		sprintf(evenRing + strlen(evenRing), "--> %d ", my_rank);		//Updates "sent list"
	} 
	else if(my_rank == 1)		//Start at process 1, go to previous odd processor, return to process 1 (odd ring)
	{
		//cout << "test test is this thing (" << my_rank << ") on" << endl; 		//Tester print statement
		MPI_Send(oddRing, strlen(oddRing) + 1, MPI_CHAR, p - (p%2) - 1, tag, MPI_COMM_WORLD); 
			//p - (p%2) - 1: p%2 checks if p is even or odd, then will do p - remainder (0 if even) - 1
		MPI_Recv(oddRing, 100, MPI_CHAR, 3, tag, MPI_COMM_WORLD, &status);
			//Receiving from 3 b/c going in reverse order, so 3 will be the odd processor right before 1
		sprintf(oddRing + strlen(oddRing), "--> %d ", my_rank);			//Updates "sent list"
	} 
	else if(p%2 == 0)			//Even number of processors
	{
		if(my_rank%2 == 0)		//Even ring
		{
			MPI_Recv(evenRing, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);	
				//my_rank - 2: receiving from the process that is two prior to my current rank (process ID number)
			sprintf(evenRing + strlen(evenRing), "--> %d ", my_rank);							//Updates "sent list"
			//cout << "test test is this thing (" << my_rank << ") on" << endl; 					//Tester print statement
			MPI_Send(evenRing, strlen(evenRing) + 1, MPI_CHAR, (my_rank + 2) % p, tag, MPI_COMM_WORLD); 
		} 
		else 					//Odd ring
		{
			MPI_Recv(oddRing, 100, MPI_CHAR, (my_rank + 2) % p, tag, MPI_COMM_WORLD, &status);
				//my_rank - 2: receiving from the process that is two prior to my current rank (process ID number)
			sprintf(oddRing + strlen(oddRing), "--> %d ", my_rank);								//Updates "sent list"
			//cout << "test test is this thing (" << my_rank << ") on" << endl; 					//Tester print statement
			MPI_Send(oddRing, strlen(oddRing) + 1, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD); 
		} 
	} 
	else 						//Odd number of processors
	{
		if(my_rank%2 == 0)		//Even ring
		{
			MPI_Recv(evenRing, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);	
				 //my_rank - 2: receiving from the process that is two prior to my current rank (process ID number)
			sprintf(evenRing + strlen(evenRing), "--> %d ", my_rank);							//Updates "sent list"
			//cout << "test test is this thing (" << my_rank << ") on" << endl; 					//Tester print statement
			MPI_Send(evenRing, strlen(evenRing) + 1, MPI_CHAR, (my_rank + 2) % (p + 1), tag, MPI_COMM_WORLD); 
		} 
		else 					//Odd ring
		{
			if(p - 2 == my_rank)
            {
                MPI_Recv(oddRing, 100, MPI_CHAR, (p+1) % (my_rank + 2), tag, MPI_COMM_WORLD, &status);
            }
            else
            {
                MPI_Recv(oddRing, 100, MPI_CHAR, (my_rank + 2) % (p+1), tag, MPI_COMM_WORLD, &status);
            }
			sprintf(oddRing + strlen(oddRing), "--> %d ", my_rank);								//Updates "sent list"
			//cout << "test test is this thing (" << my_rank << ") on" << endl; 				//Tester print statement
			MPI_Send(oddRing, strlen(oddRing) + 1, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD); 
				//my_rank - 2: receiving from the process that is two prior to my current rank (process ID number)
		} 
	} 

	if(my_rank == 0)
		cout << evenRing << endl;
	else if(my_rank == 1) 
		cout << oddRing << endl;

	// Shut down MPI
	MPI_Finalize();

	return 0;
}