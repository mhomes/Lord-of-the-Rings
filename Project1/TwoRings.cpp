//Project 1: Two Rings
//1-29-19
//By Allen Burris

#include <iostream>
#include <stdio.h>
#include <string>
#include "mpi.h" // message passing interface

// mpicxx -o blah file.cpp
// mpirun -q -np 32 blah

using namespace std;

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

	char * ring = message;

	sprintf(message, "sent List: ");
	sprintf(ring + strlen(ring), "%d ", my_rank);

	//Even Proccessors
	if (my_rank == 0) {
		MPI_Send(ring, strlen(ring) + 1, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD);
		MPI_Recv(ring, 100, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD, &status);
		cout << ring << endl;
	}
	else if (my_rank % 2 == 0) {
		MPI_Recv(ring, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
		sprintf(ring + strlen(ring), "%d ", my_rank);
		MPI_Send(ring, strlen(ring) + 1, MPI_CHAR, (my_rank + 1) % p, tag, MPI_COMM_WORLD);
	}

	//Odd Proccessors
	if (my_rank == 1) {
		MPI_Send(ring, strlen(ring) + 1, MPI_CHAR, my_rank + 2, tag, MPI_COMM_WORLD);
		MPI_Recv(ring, 100, MPI_CHAR, p - 2, tag, MPI_COMM_WORLD, &status);
		cout << ring << endl;
	}
	else if (my_rank % 2 == 1) {
		MPI_Recv(ring, 100, MPI_CHAR, my_rank - 2, tag, MPI_COMM_WORLD, &status);
		sprintf(ring + strlen(ring), "%d ", my_rank);
		MPI_Send(ring, strlen(ring) + 1, MPI_CHAR, (my_rank + 1) % p, tag, MPI_COMM_WORLD);
	}

	// Shut down MPI
	MPI_Finalize();

	return 0;
}