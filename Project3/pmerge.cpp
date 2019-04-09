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

void swap(int *a, int *b) {
	int * temp = a;
	a = b;
	b = temp;
	delete temp;
}

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
void smergymerge(int *a, int f1, int l1, int f2, int l2) {
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


void smerge(int *a, int f1, int l1, int f2, int l2, int * win, int n) {
	cout << "here are the things " << f1 << " " << l1 << " " << f2 << " " << l2 << endl;
	int len = (l1 - f1 + 1) + (l2 - f2 + 1);
	int * hold = new int[len + 1];

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

	cout << f1 << " !!~~!!~~!! " << f2 - (n / 2) << endl;
	for (int i = 0; i < len; i++) {
		//cout << i + winPos[0] <<" with hold ="<<hold[i]<< endl;
		if (f1 == 0 || (f2 - (n / 2)) == 0)
			win[i] = hold[i];
		else
			win[i + f1 + (f2 - (n / 2))] = hold[i];
	}

	delete[] hold;
}

void pmerge(int * a, int * b, int first, int mid, int last, int my_rank, int p) {
	//cout << " proccess " << my_rank << " in pmerge-1" << endl;
	int n = (last - first + 1);
	//cout<< "n" << last<< endl;
	int sampleSize = log2(n / 2);
	//cout << " proccess " << my_rank << " is moving" << endl;
	int* localSRankA = new int[sampleSize];
	//cout << " proccess " << my_rank << " is not moving" << endl;
	int* localSRankB = new int[sampleSize];

	int* SRankA = new int[sampleSize];
	int* SRankB = new int[sampleSize];
	//cout << " proccess " << my_rank << " in pmerge" << endl;
	for (int x = 0; x < sampleSize; x++) {
		localSRankA[x] = 0;
		localSRankB[x] = 0;
		SRankA[x] = 0;
		SRankB[x] = 0;
	}
	//cout << " proccess " << my_rank << " in pmerge1" << endl;
	int j = my_rank;
	for (int i = sampleSize * my_rank; i < (n / 2) + 1; i += sampleSize * p) {

		localSRankA[j] = rank(&a[mid + 1], n / 2, a[0 + i]);
		localSRankB[j] = rank(&a[0], n / 2, a[mid + 1 + i]);
		j += p;
	}

	MPI_Allreduce(localSRankA, SRankA, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(localSRankB, SRankB, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	cout << endl;

	int * mergymergeB = new int[2 * sampleSize];
	int * mergymergeA = new int[2 * sampleSize];
	for (int i = 0; i < sampleSize; i++) {
		mergymergeB[i] = i * sampleSize;
		mergymergeB[i + sampleSize] = SRankA[i];

		mergymergeA[i] = i * sampleSize;
		mergymergeA[i + sampleSize] = SRankB[i];
	}

	smergymerge(mergymergeB, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);
	smergymerge(mergymergeA, 0, sampleSize - 1, sampleSize, (2 * sampleSize) - 1);

	mergymergeB[2 * sampleSize] = n / 2;
	mergymergeA[2 * sampleSize] = n / 2;

	//HERE!!!!!!
	// The error is somewhere below
	//int * winPos = new int[n];
	//winPos[0] = 0;

	/*int * shapeSize = new int[(2 * sampleSize)-1];
	shapeSize[0] = 0;
	for (int i = 0; i < (2 * sampleSize)-1; i++) {
		shapeSize[i+1] = (((mergymergeA[i + 1] - 1) - mergymergeA[i]) + 1) + ((((mergymergeB[i + 1] - 1) + (n / 2)) - (mergymergeB[i] + (n / 2)) + 1));
	}

	for (int i = 0; i < (2 * sampleSize + 2)+1; i++) {
		if (my_rank == 0)
			cout << shapeSize[i] << " | ";
	}
	cout << endl;*/

	cout << mergymergeB[0] + (n / 2) << "-------" << (mergymergeB[0 + 1] - 1) + (n / 2) << endl;
	for (int i = my_rank; i < (2 * sampleSize); i += p) {
		cout << my_rank << " doing: ";
		//cout << shapeSize[i];
		//winPos[0]=0;
		/*for (j= 1; j <= i; j++){
			winPos[0] += shapeSize[j];
		}*/
		smerge(a, mergymergeA[i], mergymergeA[i + 1] - 1, mergymergeB[(i)] + (n / 2), mergymergeB[i + 1] - 1 + (n / 2), b, n);
	}

	cout << "hit " << my_rank << endl;
	//MPI_Allgather(a, n, MPI_INT, b, n, MPI_INT, MPI_COMM_WORLD);
	//MPI_Allreduce(b, a, n, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

	if (my_rank == 0)
		for (int i = 0; i < n; i++)
			cout << b[i] << " ";
	else {
		for (int i = 0; i < n; i++)
			cout << b[i] << " ";
		cout << my_rank << " bitches" << endl;
	}

}

void mergesort(int *a, int first, int last, int my_rank, int p) {
	cout << my_rank << endl;
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
	int * b = new int[(last - first) + 1];

	for (int x = 0; x < (last - first); x++) {
		b[x] = 0;
	}

	pmerge(a, b, first, mid, last, my_rank, p);
	if (my_rank == 1)
		delete[] b;
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
	int n = 32;

	if (my_rank == 0) {
		//int *a = new int[32];
		int a[] = { 4,6,8,9,16,17,18,19,20,21,23,25,27,29,31,32,1,2,3,5,7,10,11,12,13,14,15,22,24,26,28,30 };
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
		int *a = new int[n];
		MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);
		mergesort(a, 0, n - 1, my_rank, p);

	}

	cout << endl << endl << endl;

	// Shut down MPI
	cout << my_rank << " saying bye..." << endl;
	MPI_Finalize();

	//delete[] input;
	//delete[] a;

	return 0;
}