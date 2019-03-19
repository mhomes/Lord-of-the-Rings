/*(
Project 3: pmerge
Parrallel Algo - Spring 2019
Project by: Allen Burris and Mathew Homes
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <fstream>
#include "mpi.h" // message passing interface

// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

using namespace std;
int rank(int *a, int first, int last, int ValToFind) {

	if (a[first + (last - first) / 2] == ValToFind)
		return (first + (last - first) / 2);
	else if (first == last - 1)
		return first;
	else if (a[first + (last - first) / 2] > ValToFind)
		return rank(a, first, first + (last - first) / 2, ValToFind);
	else if (a[first + (last - first) / 2] < ValToFind)
		return rank(a, first + (last - first) / 2, last, ValToFind);

}

//checked
void smerge(int *a, int f1, int l1, int f2, int l2) {
	int * hold = new int[l2 + 1];
	int len = (l2 - f1) + 1;

	for (int i = 0; i < len+1; i++)
		if (f1 <= l1 || f2 <= l2)
			if (a[f1] < a[f2])
				hold[i] = a[f1++];
			else if (a[f1] > a[f2])
				hold[i] = a[f2++];

	cout << endl;
	for (int i = 0; i < l2; i++) {
		a[i] = hold[i];
		cout << a[i] << " ";
	}

	delete[] hold;
}

void pmerge(int * a , int * b, int first , int last, int mid, int my_rank, int p){
	int n = (last - first + 1);
	int sampleSize = log2(n / 2);
	int* localSRankA = new  int [sampleSize];
	int* localSRankB = new  int [sampleSize];
	int* SRankA = new  int [sampleSize];
	int* SRankB = new  int [sampleSize];
 
 
	for (int x = 0; x < sampleSize; x++){
		localSRankA[x]= 0;
		localSRankB[x]= 0;
		SRankA[x]= 0;
		SRankB[x]= 0;
	}
	
	
	for (int i = sampleSize * my_rank; i < n; i += sampleSize*p){
		int j = my_rank;
		localSRankA[j] = rank(&a[mid+1], mid + 1, last, a[0+j]);
		localSRankB[j] = rank(&a[0], first, mid, a[mid+1+j]);
		j+=p;
	}
	
	MPI_Allreduce(&localSRankA, &SRankA, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(&localSRankB, &SRankB, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	for (int x = 0; x< sampleSize ; x++){
		cout << SRankA[x]<< endl;
		cout << SRankB[x]<< endl;
	}
}		

void mergesort(int *a, int first, int last, int my_rank, int p) {
	if ((last - first ) <= 8) return; //(last <= first) return;
	if (last == first + 1) {
		if (a[first] < a[last]) swap(a[first], a[last]);
		return; }
	int mid = (first + last) / 2;
	mergesort(a, first, mid, my_rank, p);
	mergesort(a, mid + 1, last, my_rank, p);
	int * b = new int[last - first + 1];
	pmerge(a, b, first, mid, last, my_rank, p); 
	}

int main(int argc, char * argv[]) {

	int my_rank;			// my CPU number for this process
	int p;					// number of CPUs that we have
	int source;				// rank of the sender
	int dest;				// rank of destination
	int tag = 0;			// message number
	char message[100];		// message itself
	MPI_Status status;		// return status for receive

	// Start MPI
	MPI_Init(&argc, &argv);

	// Find out my rank!
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Find out the number of processes!
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	// The real program is here

	int * input = NULL;
	int n;

	/* Here is the parrallel version for when smerge and rank are working.
	if (my_rank == 0) {
		cout << "Welcome to the pMerge Program. \n Please enter the size of the array you wish to sort:" << endl;
		cin >> n;
		srand(time(0));
		input = new int[n];
		cout << endl << "The unsorted array: " << endl;
		for (int x = 0; x < n; x++) {
			input[x] = rand() % 100000;
			cout << input[x] << " ";
		}
		cout << endl;
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (my_rank != 0)
		input = new int[n];
	MPI_Bcast(input, n, MPI_INT, 0, MPI_COMM_WORLD);
	*/
if (my_rank= 0) {
	int *a = new int[16];
	//a = { 1,3,5,7,9,11,13,15,2,4,6,8,10,12,14,16 };
	n = 16;
	for (int x = 0; x < n; x++)
		a[x] = x+1;
	MPI_Bcast(&a, n, MPI_INT, 0, MPI_COMM_WORLD);
	
	mergesort(a, 0, n-1, my_rank, p);
	if (my_rank == 0)
		for (int i = 0; i < 16; i++)
			cout << a[i] <<endl;

	cout << endl;
}
else{
	int *a = new int[16];
	n = 16;
	MPI_Bcast(&a, n, MPI_INT, 0, MPI_COMM_WORLD);
	
	mergesort(a, 0, n-1, my_rank, p);
	if (my_rank == 0)
		for (int i = 0; i < 16; i++)
			cout << a[i] <<endl;

	cout << endl;

}
/*
	int c[20] = { 1,2,4,5,6,7,9,10,12,14,15,17,20,21,40,50,55,56,59,60 };
	int * d = new int[20];

	for (int i = 0; i < 20; i++)
		d[i] = c[i];

	int XD = rank(d, 0, 19, 8);
	cout << XD << endl;

	for (int i = 0; i < 16; i++)
		a[i] = b[i];
	cout << endl;
*/
	

	//int * rankA = new int[log];
	//int * rankB = new int[log];



	

	// Shut down MPI
	MPI_Finalize();

	//delete[] input;
	//delete[] a;

	return 0;
}