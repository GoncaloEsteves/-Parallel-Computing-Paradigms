#include <mpi.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "PrimeServer.cpp"


void generate(int min, int max, int* buf) {
  	int j = 0;
  	for(int i = min; i < max; i += 2){
    	buf[j++] = i;
  	}
}

int main(int argc, char **argv) {
  	int nprocesses; 
  	int myrank; 
 
  	MPI_Init (&argc, &argv); 
 
  	MPI_Comm_size (MPI_COMM_WORLD, &nprocesses); 
  	MPI_Comm_rank (MPI_COMM_WORLD, &myrank); 
 
  	if(nprocesses == 3){
      
      	int MAXP = 1000000;
      	int SMAXP = 1000;
      	int pack = MAXP/10;
      	int *ar = new int[pack/2];
		MPI_Status status;

    	if(myrank == 0){
      		PrimeServer *ps1 = new PrimeServer();

      		ps1 -> initFilter(1, SMAXP/3, SMAXP);

      		for(int i = 0; i < 10; i++) {
          		generate(i*pack, (i+1)*pack, ar);
          		ps1->mprocess(ar,pack/2);

			    MPI_Send(ar, pack/2, MPI_INT, myrank+1, 0, MPI_COMM_WORLD);
      		}
    	}

    	else if(myrank == 1){
      		PrimeServer *ps2 = new PrimeServer();

      		ps2 -> initFilter(SMAXP/3+1, 2*SMAXP/3, SMAXP);

      		for(int i = 0; i < 10; i++) {
				MPI_Recv(ar, pack/2, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, &status);

          		ps2->mprocess(ar,pack/2);

				MPI_Send(ar, pack/2, MPI_INT, myrank+1, 0, MPI_COMM_WORLD);
      		}
    	}

    	else{
      		PrimeServer *ps3 = new PrimeServer();

      		ps3 -> initFilter(2*SMAXP/3+1, SMAXP, SMAXP);

      		for(int i = 0; i < 10; i++) {
				MPI_Recv(ar, pack/2, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, &status);

          		ps3->mprocess(ar,pack/2); 
      		}

      		ps3->end();
    	}
  	}

  	else if(nprocesses == 1){
      
      	int MAXP = 1000000;
      	int SMAXP = 1000;
      
      	PrimeServer *ps1 = new PrimeServer();
      	PrimeServer *ps2 = new PrimeServer();
      	PrimeServer *ps3 = new PrimeServer();
      
      	ps1 -> initFilter(1, SMAXP/3, SMAXP);
      	ps2 -> initFilter(SMAXP/3+1, 2*SMAXP/3, SMAXP);
      	ps3 -> initFilter(2*SMAXP/3+1, SMAXP, SMAXP);
      
      	int pack = MAXP/10;
      	int *ar = new int[pack/2];

      	for(int i = 0; i < 10; i++) {
          	generate(i*pack, (i+1)*pack, ar);
          	ps1->mprocess(ar,pack/2);
          	ps2->mprocess(ar,pack/2);
          	ps3->mprocess(ar,pack/2);
      	}
        
      	ps3->end();
  	}

  	MPI_Finalize ();
 	return(0);
}