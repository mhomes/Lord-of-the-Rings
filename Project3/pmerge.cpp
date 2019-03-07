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

	int *a = new int[16];
	int b[16] = { 1,3,5,7,9,11,13,15,2,4,6,8,10,12,14,16 };

	int c[20] = { 1,2,4,5,6,7,9,10,12,14,15,17,20,21,40,50,55,56,59,60 };
	int * d = new int[20];

	for (int i = 0; i < 20; i++)
		d[i] = c[i];

	int XD = rank(d, 0, 19, 8);
	cout << XD << endl;

	for (int i = 0; i < 16; i++)
		a[i] = b[i];
	cout << endl;

	int log = log2(n / 2);

	int * rankA = new int[log];
	int * rankB = new int[log];



	smerge(a, 0, 7, 8, 15);
	/*if (my_rank == 0)
		for (int i = 0; i < 16; i++)
			cout << a[i] <<endl;*/

	cout << endl;

	// Shut down MPI
	MPI_Finalize();

	delete[] input;
	delete[] a;

	return 0;
}
