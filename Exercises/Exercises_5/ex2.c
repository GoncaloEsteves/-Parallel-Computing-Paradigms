#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rank, msg, N;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &N);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		msg = 123456;
		MPI_Send(&msg, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
		printf("Sent %d\n", rank);
	}
	else if (rank == N-1) {
		MPI_Recv(&msg, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
		printf("Received %d %d\n", rank, msg);
	}
	else {
		MPI_Recv(&msg, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
		printf("Received %d %d\n", rank, msg);

		MPI_Send(&msg, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
		printf("Sent %d\n", rank);
	}
	
	MPI_Finalize();
	return 0;
}