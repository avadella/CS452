#include <iostream>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 
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
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number (ID number for a given message in a chain)
	char message[100];		// message itself
	MPI_Status status;		// return status for receive
	
	// Start MPI
	MPI_Init(&argc, &argv);
	
	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	
	//Step 1: KNOW THE PROBLEM - only one processor (process 0) needs to have all the input 
	srand(71911); 						//Creating a seed for random numbers 
	int n = 1000000;
	int * array = new int[n]; 			//Dynamically allocated array
	long int minimum = 0;				//Variable to actually store the minimum value
	long int maximum = 0;				//Variable to actually store the maximum value
	long int average = 0;				//Variable to actually store the average value
	
	//Fill in the array with random numbers on process 0 (irrelevant to do this on other processors)
	if(my_rank == 0)
		for(int i = 0; i < n; i++) {
			array[i] = 1 + (rand() % 100);
			//cout << array[i] << " ";			//Tester print statement to see values in the array
		} 
	
	//Step 2: DIVIDE THE PROBLEMS UP INTO PIECES - use MPI_Scatter to scatter the array into to each process
	int localn = n/p;					//Amount of work each processor will do
	int * localarray = new int[localn];
	
	MPI_Scatter(&array[0], localn, MPI_INT, localarray, localn, MPI_INT, 0, MPI_COMM_WORLD); 
	
	//Step 3: DO THE LOCAL WORK - have each process compute a partial answer
	//Having each local value start at the first position of the local array
	int localmaximum = localarray[0];
	int localminimum = localarray[0];
	long int localaverage = 0;
	
	for(int i = 0; i < localn; i++)	{		//For loop to calculate local maximum
		if(localarray[i] > localmaximum)
			localmaximum = localarray[i];
	} 
	
	for(int i = 0; i < localn; i++)	{		//For loop to calculate local minimum
		if(localarray[i] < localminimum)
			localminimum = localarray[i];
	}
	
	for(int i = 0; i < localn; i++)			//For loop to calculate local average
		localaverage += localarray[i];
	
	//Step 4: COLLECT THE RESULTS BACK TOGETHER - use AllReduce to reduce the answers together on process 0
	MPI_Reduce(&localmaximum, &maximum, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	MPI_Reduce(&localminimum, &minimum, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
	MPI_Reduce(&localaverage, &average, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if(my_rank == 0) {
		cout << "Maximum: " << maximum << endl;
		cout << "Minimum: " << minimum << endl;
		cout << "Average: " << (double)average/n << endl;
	} 
	
	//Deleting dynamically allocated arrays
	delete [] array;
	delete [] localarray;
	
	// Shut down MPI
	MPI_Finalize();

	return 0;
}