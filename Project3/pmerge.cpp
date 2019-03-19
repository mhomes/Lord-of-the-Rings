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

int rank (int *a, int size, int ValToFind) {
	if (size == 1){
		if( ValToFind < a[0])
			return 0;
		else
			return 1;
		}
	else if ( ValToFind < a[size/2])
		return rank(a,size/2, ValToFind);
	else
		return size/2 + rank(&a[size/2], size/2, ValToFind);
		
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

void pmerge(int * a , int * b, int first , int mid, int last, int my_rank, int p){
	int n = (last - first + 1);
	cout<< "n" << last<< endl;
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
	int j = my_rank;
	for (int i = sampleSize * my_rank; i < (n/2)+1; i += sampleSize*p){
		cout <<endl << a[0 +i]<< endl;
		
		cout<< "sampleSize" << sampleSize<< endl;
		 //why are we passing only 1 value of a[], rank() takes the whole array
		localSRankA[j] = rank(&a[mid+1],n/2, a[0+i]); // first half of the array, in project description
		cout << j << my_rank<<":"<<localSRankA[j]<< endl;
		cout <<endl << a[mid+1+i]<< endl;
		localSRankB[j] = rank(&a[0], n/2, a[mid+1+i]); // second half of the array, in project description
		cout << j << my_rank <<":"<<localSRankB[j]<< endl;
		j+=p;
	}

	MPI_Allreduce(localSRankA, SRankA, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	MPI_Allreduce(localSRankB, SRankB, sampleSize, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	for (int x = 0; x < sampleSize; x++){
		cout<<SRankA[x];
		cout<<SRankB[x];
	}
	
}		

void mergesort(int *a, int first, int last, int my_rank, int p) {
	if ((last - first ) <= 10) return; //(last <= first) return;
		if (last == first + 1) {
			if (a[first] < a[last]) swap(a[first], a[last]);
				return; }
		int mid = (first + last) / 2;
		mergesort(a, first, mid, my_rank, p);
		mergesort(a, mid + 1, last, my_rank, p);
		int * b = new int[last - first + 1];
		
		for (int x = 0; x < (last - first); x++){
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

	
	if (my_rank== 0) {
		int *a = new int[16];
		//a = { 1,3,5,7,9,11,13,15,2,4,6,8,10,12,14,16 };
		n = 16;
		for (int x = 0; x < n/2; x++)
			a[x] = 2 * x + 1;
		
		for (int x = 1; x < (n/2)+1; x++)
			a[x+7] = 2 * x;
		
		for (int x = 0; x < n; x++){
			cout<< a[x] << " ";
		}
		MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);
	
		mergesort(a, 0, n-1, my_rank, p);
		if (my_rank == 0)
			for (int i = 0; i < 16; i++)
				//cout << a[i] <<endl;

		cout << endl;
	}
	else{
		int *a = new int[16];
		n = 16;
		MPI_Bcast(a, n, MPI_INT, 0, MPI_COMM_WORLD);
		mergesort(a, 0, n-1, my_rank, p);
	
	}

	// Shut down MPI
	MPI_Finalize();

	//delete[] input;
	//delete[] a;

	return 0;
}