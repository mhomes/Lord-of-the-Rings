/*(
Project 3: pmerge
Parrallel Algo - Spring 2019
Github version
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
/*int rank(int *a, int first, int last, int ValToFind) {

	if (a[first + (last - first) / 2] == ValToFind)
		return (first + (last - first) / 2);
	else if (first == last - 1)
		return first;
	else if (a[first + (last - first) / 2] > ValToFind)
		return rank(a, first, first + (last - first) / 2, ValToFind);
	else if (a[first + (last - first) / 2] < ValToFind)
		return rank(a, first + (last - first) / 2, last, ValToFind);

}*/

int rank(int *a, int size, int ValToFind) {
	if (size == 1) {
		if (ValToFind < a[0])
			return 0;
		else
			return 1;
	}
	else if (ValToFind < a[size / 2])
		return rank(a, size / 2, ValToFind);
	else
		return size / 2 + rank(&a[size / 2], size / 2, ValToFind);

}

//checked
void smerge(int *a, int f1, int l1, int f2, int l2) {
	cout << "here are the things " << f1 << " " << l1 << " " << f2 << " " << l2 << endl;
	int len = (l1 - f1 + 1) + (l2 - f2 + 1);
	int * hold = new int[len];

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

void pmerge(int * a, int * b, int first, int mid, int last, int my_rank, int p) {
	int n = (last - first + 1);
	//cout<< "n" << last<< endl;
	int sampleSize = log2(n / 2);
	int* localSRankA = new  int[sampleSize];
	int* localSRankB = new  int[sampleSize];
	int* SRankA = new  int[sampleSize];
	int* SRankB = new  int[sampleSize];

	for (int x = 0; x < sampleSize; x++) {
		localSRankA[x] = 0;
		localSRankB[x] = 0;
		SRankA[x] = 0;
		SRankB[x] = 0;
	}
	int j = my_rank;
	for (int i = sampleSize * my_rank; i < (n / 2) + 1; i += sampleSize * p) {
		//cout <<endl << a[0 +i]<< endl;

		//cout<< "sampleSize" << sampleSize<< endl;

		localSRankA[j] = rank(&a[mid + 1], n / 2, a[0 + i]);
		//cout << j << my_rank<<":"<<localSRankA[j]<< endl;
		//cout <<endl << a[mid+1+i]<< endl;
		localSRankB[j] = rank(&a[0], n / 2, a[mid + 1 + i]);
		//cout << j << my_rank <<":"<<localSRankB[j]<< endl;
		j += p;
	}
	cout << j << endl;

	MPI_Allreduce(localSRankA, SRankA, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(localSRankB, SRankB, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	for (int x = 0; x < sampleSize; x++) {
		cout << SRankA[x] << " ";
	}
	cout << endl;

	for (int x = 0; x < sampleSize; x++) {
		cout << SRankB[x] << " ";
	}

	j = 0;
	/*for (int i = 0; i < (n / 2) + 1; i += 2*sampleSize) {
		b[i + SRankA[j]] = a[i];
		j++;
	}
	j = 0;
	for (int i = 0; i < (n / 2) + 1; i += 2*sampleSize) {
		b[i + SRankB[j]] = a[(n/2)+i];
		j++;
	}

	if(my_rank == 0)
	for (int i = 0; i < n; i += 2*sampleSize) {
		cout <<"Test on "<<my_rank<<" "<<  b[i] << endl;
	}*/

	//if(my_rank == 0)
	/*if(my_rank == 0)
		//for (int i = 0; i < (n / 2) + 1; i += 2*sampleSize) {
		for (int i = 0; i < n; i += 2*sampleSize) {
		// need to stripe it, cant have process zero do every shape ( can wait till shapes are done)
		// this assumes that each array is size 6, need to find a way to find exact end points of each shape
			// look at problem in the book

		// harry recomends that we may need to redo the for loop
			cout << i<< " " << SRankB[j] << " " << (n / 2) + i << " " << n/2 + SRankA[(j)] << endl;
			smerge(a, i , SRankB[j], (n / 2) + i, n/2 + SRankA[(j)]);
			cout << "finished smerge " << i << endl;
			j++;
		}*/
	cout << endl;

	int * mergymergeB = new int[2 * sampleSize];
	int * mergymergeA = new int[2 * sampleSize];
	for (int i = 0; i < sampleSize; i++) {
		mergymergeB[i] = i * sampleSize;
		mergymergeB[i + sampleSize] = SRankA[i];

		mergymergeA[i] = i * sampleSize;
		mergymergeA[i + sampleSize] = SRankB[i];
	}

	smerge(mergymergeB, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);
	smerge(mergymergeA, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);

	mergymergeB[2 * sampleSize] = n / 2;
	mergymergeA[2 * sampleSize] = n / 2;

	for (int i = 0; i <= 2 * sampleSize; i++)
		cout << mergymergeA[i] << " ";
	cout << endl;
	for (int i = 0; i <= 2 * sampleSize; i++)
		cout << mergymergeB[i] << " ";

	//HERE!!!!!!
	cout << "hit" << endl;
	cout << mergymergeA[0 + 1] << endl;
	for (int i = 0; i < 1; i++) {
		smerge(a, mergymergeA[i], mergymergeA[i + 1] - 1, mergymergeB[(i + (n / 2))], mergymergeB[i + (n / 2) + 1] - 1);
	}

}

void mergesort(int *a, int first, int last, int my_rank, int p) {
	if ((last - first) <= 16)
		return; //(last <= first) return;
	if (last == first + 1) {
		if (a[first] < a[last])
			swap(a[first], a[last]);
		return;
	}
	int mid = (first + last) / 2;
	mergesort(a, first, mid, my_rank, p);
	mergesort(a, mid + 1, last, my_rank, p);
	int * b = new int[last - first + 1];

	for (int x = 0; x < (last - first); x++) {
		b[x] = 0;
	}

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

	if (my_rank == 0) {
		//int *a = new int[32];
		int a[] = { 4,6,8,9,16,17,18,19,20,21,23,25,27,29,31,32,1,2,3,5,7,10,11,12,13,14,15,22,24,26,28,30 };
		n = 32;
		/*for (int x = 0; x < n/2; x++)
			a[x] = 2 * x + 1;

		for (int x = 1; x < (n/2)+1; x++)
			a[x+7] = 2 * x;
		if(my_rank == 0)
			for (int x = 0; x < n; x++){
				cout<< a[x] << " ";
			}
		*/

		MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);

		mergesort(a, 0, n - 1, my_rank, p);
	}
	else {
		int *a = new int[16];
		n = 16;
		MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);
		mergesort(a, 0, n - 1, my_rank, p);

	}

	// Shut down MPI
	MPI_Finalize();

	//delete[] input;
	//delete[] a;

	return 0;
}