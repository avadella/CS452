#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
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
	
	//Step 1: KNOW THE PROBLEM - Only one processor (process 0) will read in the contents of the .txt file 
	//Start by counting how many characters are in the .txt file (store amount in a count var)
	char txtChar; 
	int count = 0;
	ifstream infile("Anna-Eye.txt");
	while (infile >> txtChar) 				//Loop reads contents of the .txt file one char at a time, skipping white spaces
		count++; 
		
	//Create a char array, and fill in that array on processor 0 with the letters from the .txt file
	int n = count;
	char * txtLetters = new char[n];		//char array
	int letterCount[26] = {0}; 				//array to count each letter, set all array entries to 0
	
	if(my_rank == 0) {						//filling in txtLetters array w/ letters from .txt on processor 0
		ifstream infile("Anna-Eye.txt");
		infile >> txtLetters;
	} 
	
	//Step 2: DIVIDE THE PROBLEMS UP INTO PIECES - Use MPI_Scatter to scatter the info in the .txt file across all processors
	int localn = n/p;						//Amount of work each processor will do
	char * localarray = new char[localn]; 
	int localLetterCount[26] = {0};			//local array to count each letter, set all array entries to 0
	
	//Scatter info from .txt file from process 0 to all other processors
	MPI_Scatter(&txtLetters[0], localn, MPI_CHAR, localarray, localn, MPI_CHAR, 0, MPI_COMM_WORLD); 
	
	//Step 3: DO THE LOCAL WORK - "use exactly one loop" and "this loop will not have a single conditional in it at all"
	for(int i = 0; i < localn; i++)
		localLetterCount[(int)localarray[i] - 97]++; 	//ASCII table: lowercase letter alphabet start at 97 w/ 'a'
	
	//Step 4: COLLECT THE RESULTS BACK TOGETHER - Gather/Reduce the 26 counts back together
	for (int i = 0; i < 26; i++)			//Reduce 
		MPI_Reduce(&localLetterCount[i], &letterCount[i], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	
	if(my_rank == 0) {
		for(int i = n - (n%p); i < n; i++)
			letterCount[(int)txtLetters[i] - 97]++;	   //typecasting the txtLetters as ints for the total count
		
		int totalLetterCount = 0;			//Creating a new count var to keep track of the total count
		
		for(int i = 0; i < 26; i++) {
			cout << (char)(i + 97) << ": " << letterCount[i] << endl; 	//Typecasting as char
			totalLetterCount += letterCount[i];
		} 
		
	cout << "Total Letters: " << totalLetterCount << endl; 	 //Printing out the total amount of letters in the .txt file
	
	}

	//Deleting dynamically allocated arrays 
	delete [] txtLetters; 
	delete [] localarray;

	// Shut down MPI
	MPI_Finalize();

	return 0;
}