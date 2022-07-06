#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rank, msg, N;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &N);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
		for(int i = 0; i < 10; i++){
			MPI_Send(&i, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			printf("%dnd message sent %d\n", i, rank);
		}
	}
	else if (rank == N-1) {
		for (int i = 0; i < 10; i++){
			MPI_Recv(&msg, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
			printf("%dnd message received %d %d\n", i, rank, msg);
		}
	}
	else {
		for (int i = 0; i < 10; i++){
			MPI_Recv(&msg, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, &status);
			printf("%dnd message received %d %d\n", i, rank, msg);

			MPI_Send(&msg, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD);
			printf("%dnd message sent %d\n", i, rank);
		}
	}
	
	MPI_Finalize();
	return 0;
}