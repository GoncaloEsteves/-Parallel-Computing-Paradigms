#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int rank, msg;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	/* Process 0 sends and Process 1 receives */
	if (rank == 0) {
		msg = 123456;
		MPI_Send(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		printf("Sent 0\n");
	}
	else if (rank == 1) {
		MPI_Recv(&msg, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
		printf("Received 1 %d\n", msg);

		MPI_Send(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		printf("Sent 1\n");
	}
	else if (rank == 2) {
		MPI_Recv(&msg, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
		printf("Received 2 %d\n", msg);

		MPI_Send(&msg, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		printf("Sent 2\n");
	}
	else if (rank == 3) {
		MPI_Recv(&msg, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, &status);
		printf("Received 3 %d\n", msg);
	}
	
	MPI_Finalize();
	return 0;
}