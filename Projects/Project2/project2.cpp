//CS452 Project 2: A Minor Character Can Still Drink Heavily
//Written by Anna Vadella

#include <iostream>
#include <string.h>
using namespace std; 

//Merge function facilitates the merging of two sorted arrays 
void merge(int * a, int * b, int lasta, int lastb, int * output = NULL) 
{
	int i = 0; 
    int j = 0;
    int k = 0;
	
    while(i <= lasta && j <= lastb) {
        if (a[i] < b[j]) 
            output[k++] = a[i++]; 
        else
            output[k++] = b[j++]; 
    }

    while (i <= lasta) 
        output[k++] = a[i++]; 
    while (j <= lastb) 
        output[k++] = b[j++];
} 

//Mergesort function must be recrusive, less than 10 lines of code (added output arameter)
void mergesort(int * a, int first, int last, int * output = NULL) 
{	
	//Base Case
	if(last - first < 1)
		return;
	
	//Divides the array in half
	int middle = (first + last)/2;
	
	//cout << "Here " << first << " " << last << endl;		//Debugging print statement
	
	mergesort(a, first, middle, output);					//First half
	mergesort(a, middle + 1, last, output);					//Second half
	
	//Combine both halves together
	merge(&a[first], &a[middle + 1], middle - first, last - (middle + 1), &output[first]);	

	//Copy results from array a to output
    for (int i = first; i <= last; i++)
        a[i] = output[i];
} 

//Helper function to print array
void printArray(int * a, int size)
{
	for(int i = 0; i < size; i++) {
		cout << a[i] << " "; 
	}
	
	cout << endl;
} 

int main()
{
	//Define parameter a based on a user inputted size 
	int arraySize = 0; 
	cout << "Enter an array size: ";
	cin >> arraySize;	
	
	//Initialize dynamically allocated arrays
	int * userArray = new int[arraySize];
	int * outputArray = new int[arraySize];
	
	//Fill array with random numbers, have numbers within the array be between 0 and 1000000
	srand(time(0));
	for(int i = 0; i < arraySize; i++) {
		userArray[i] = 1 + (rand() % 1000000); 
	}
	
	//Printing out the values within array before merging process
	cout << "Array before merge sort: " << endl;
	printArray(userArray, arraySize);
	
	//Merging process happens here
	mergesort(userArray, 0, arraySize - 1, outputArray);
	
	//Printing out the values within array after merging process
	cout << "Array after merge sort: " << endl;
	printArray(outputArray, arraySize);
	
	//Deleting dynamically allocated array
	delete [] userArray;
	delete [] outputArray;
	
	return 0;
} 