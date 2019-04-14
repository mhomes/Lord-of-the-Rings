/*(
Project 3: pmerge
Parrallel Algo - Spring 2019
Thomas Version version
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

void swap(int *a, int *b) {
	int * temp = a;
	a = b;
	b = temp;
}

int rank(int *a, int size, int valToFind) {
	if (size == 1)
		if (valToFind < a[0])
			return 0;
		else
			return 1;
	else if (valToFind < a[size / 2])
		return rank(a, size / 2, valToFind);
	else
		return (size / 2) + rank(&a[size / 2], size / 2, valToFind);
}

void smerge(int *a, int f1, int l1, int f2, int l2, int *win, int n) {
	int len = (l1 - f1 + 1) + (l2 - f2 + 1);
	int *hold = new int[len + 1];
	int tempf1 = f1;
	int tempf2 = f2;

	int i = 0;
	while (f1 <= l1 && f2 <= l2)
		if (a[f1] <= a[f2])
			hold[i++] = a[f1++];
		else
			hold[i++] = a[f2++];
	while (f1 <= l1)
		hold[i++] = a[f1++];
	while (f2 <= l2)
		hold[i++] = a[f2++];

	f1 = tempf1;
	f2 = tempf2;

	for (int i = 0; i < len; i++)
		win[i + f1 + (f2 - (n / 2))] = hold[i];

	delete[] hold;
}

void endmerge(int *a, int f1, int l1, int f2, int l2) {
	int len = (l1 - f1 + 1) + (l2 - f2 + 1);
	int *hold = new int[len];

	int i = 0;
	while (f1 <= l1 && f2 <= l2)
		if (a[f1] <= a[f2])
			hold[i++] = a[f1++];
		else
			hold[i++] = a[f2++];
	while (f1 <= l1)
		hold[i++] = a[f1++];

	while (f2 <= l2)
		hold[i++] = a[f2++];

	for (int i = 0; i < len; i++)
		a[i] = hold[i];
	delete[] hold;
}

void pmerge(int * a, int * b, int first, int mid,int last,int my_rank,int p) {

	int n = (last - first) + 1;
	int sampleSize = log2(n / 2);
	cout << " n = " << n << endl;
	int * localSRankA = new int[sampleSize];
	int * localSRankB = new int[sampleSize];

	int * SRankA = new int[sampleSize];
	int * SRankB = new int[sampleSize];

	for (int i = 0; i < sampleSize; i++) {
		localSRankA[i] = -1;
		localSRankB[i] = -1;
		SRankA[i] = -1;
		SRankB[i] = -1;
	}

	int j = my_rank;
	for (int i = (sampleSize * my_rank); i < n / 2; i += sampleSize*p) {
		localSRankA[j] = rank(&a[mid + 1], n / 2, a[0 + i]);
		localSRankB[j] = rank(&a[0], n / 2, a[mid + 1 + i]);
		j += p;
	}
	cout <<endl<< "sample size is:" << sampleSize<<endl;
	cout << "ranks are: ";
	for (int i = 0; i < sampleSize; i++)
		cout << localSRankB[i] << " ";

	MPI_Allreduce(localSRankA, SRankA, sampleSize, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(localSRankB, SRankB, sampleSize, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	cout<<":" << endl;

	int * endPointsA = new int[2 * sampleSize];
	int * endPointsB = new int[2 * sampleSize];
	for (int i= 0; i < sampleSize; i++) {
		endPointsA[i] = i * sampleSize;
		endPointsA[i + sampleSize] = SRankB[i];

		endPointsB[i] = i * sampleSize;
		endPointsB[i + sampleSize] = SRankA[i];
	}

	endmerge(endPointsA, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);
	endmerge(endPointsB, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);

	endPointsA[sampleSize] = n / 2;
	endPointsB[sampleSize] = n / 2;


	for (int i = 0; i <= 2 * sampleSize; i++)
		cout << endPointsA[i] << " ";
	cout << endl;
	for (int i = 0; i <= 2 * sampleSize; i++)
		cout << endPointsB[i] << " ";
	cout << endl;

	for (int i = my_rank; i < 2 * sampleSize; i += p) {
		smerge(a, endPointsA[i], endPointsA[i + 1] - 1, endPointsB[i] + n / 2, endPointsB[i + 1] - 1 + (n / 2), b, n);
		cout << i << endl;
	}

}

void mergesort(int *a, int first, int last, int my_rank, int p) {
	if ((last - first) <= 8)
		return;
	if (last == first + 1) {
		if (a[first] < a[last])
			swap(a[first], a[last]);
		return;
	}

	int mid = (first + last) / 2;
	mergesort(a, first, mid, my_rank, p);
	mergesort(a, mid + 1, last, my_rank, p);

	int *b = new int[(last - first) + 1];
	for (int i = 0; i < (last - first); i++)
		b[i] = -1;
	
	pmerge(a, b, first, mid, last, my_rank, p);

	delete[]b;
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

	int n = 16;
	int a[] = { 20,21,23,25,27,29,31,32,4,6,8,9,16,17,18,19};

	if (my_rank == 0) {
		;
	}
	MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);

	for (int i = 0; i < n; i++) {
		cout << a[i] << " ";
	}

	mergesort(a,0,n-1, my_rank,p);

	cout<<endl << my_rank << " saying bye..." << endl;
	MPI_Finalize();

	return 0;
}