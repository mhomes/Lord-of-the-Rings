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

//Normal Smerge
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
	return;
}

//smerge for endpoints
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

	//Calculate the Ranks
	int j = my_rank;
	for (int i = (sampleSize * my_rank); i < n / 2; i += sampleSize*p) {
		localSRankA[j] = rank(&a[n/2], n / 2, a[0 + i]);
		localSRankB[j] = rank(&a[0], n / 2, a[mid + i+1]);
		j += p;
	}

	MPI_Allreduce(localSRankA, SRankA, sampleSize, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
	MPI_Allreduce(localSRankB, SRankB, sampleSize, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	//combine ranks and rank values for endpoints
	int * endPointsA = new int[(2 * sampleSize)+1];
	int * endPointsB = new int[(2 * sampleSize)+1];
	for (int i= 0; i < sampleSize; i++) {
		endPointsA[i] = i * sampleSize;
		endPointsA[i + sampleSize] = SRankB[i];

		endPointsB[i] = i * sampleSize;
		endPointsB[i + sampleSize] = SRankA[i];
	}

	endmerge(endPointsA, 0, sampleSize - 1, sampleSize, (2 * sampleSize)-1);
	endmerge(endPointsB, 0, sampleSize - 1, sampleSize, (2 * sampleSize)-1);

	endPointsA[2*sampleSize] = n / 2;
	endPointsB[2*sampleSize] = n / 2;

	//Distribute the Smerge
	int i = my_rank;
	while (i < 2 * sampleSize) {
		smerge(a, endPointsA[i], endPointsA[i + 1] - 1, endPointsB[i] + n / 2, endPointsB[i + 1] - 1 + (n / 2), b, n);
		i += p;
	}

	//Fits sorted array into correct position in main array. 
	int * c = new int[n];
	for (int i = 0; i < n; i++)
		c[i] = -1;
	MPI_Allreduce(b, c, n, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	for (int i = first; i < n; i++)
		a[i] = c[i];

	delete[] c;
}

void mergesort(int *a, int first, int last, int my_rank, int p) {
	//Base Case of 4
	if ((last - first) <= 4) {
		return;
	}
	if (last == first + 1) {
		//naver used cause Base case of 4
		if (a[first] > a[last])
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
//This is a function
// This is a comment.
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

	int n = 64;
	int * a = new int[n];

	if (my_rank == 0) {
		int b[32] = { 4,6,8,9,16,17,18,19,20,21,23,25,27,29,31,32,1,2,3,5,7,10,11,12,13,14,15,22,24,26,28,30 };
		for (int i = 0; i < 32; i++) {
			a[i] = b[i];
			a[i + 32] = a[i] + 32;
		}
	}
	MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);

	if (my_rank == 0) {
		cout << "The array before sorting" << endl;
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}
	}

	mergesort(a,0,n-1, my_rank,p);

	if (my_rank == 0) {
		cout << "The array After sorting" << endl;
		for (int i = 0; i < n; i++) {
			cout << a[i] << " ";
		}
	}

	cout<<endl << my_rank << " saying bye..." << endl;
	MPI_Finalize();

	return 0;
}