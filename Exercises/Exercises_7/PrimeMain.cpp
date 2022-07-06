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
		double timei, timef, time = 0, timeim, timefm, timem = 0;

    	if(myrank == 0){
      		PrimeServer *ps1 = new PrimeServer();

      		ps1 -> initFilter(1, 100, SMAXP);

      		for(int i = 0; i < 10; i++) {
          		generate(i*pack, (i+1)*pack, ar);

          		timei = MPI_Wtime();
          		ps1->mprocess(ar,pack/2);
          		timef = MPI_Wtime();
          		time += (timef-timei)/1000;

          		timeim = MPI_Wtime();
			    MPI_Send(ar, pack/2, MPI_INT, myrank+1, 0, MPI_COMM_WORLD);
          		timefm = MPI_Wtime();
          		timem += (timefm-timeim)/1000;
      		}
          	
        	std::cout << "O process 0 demorou " << time << "s e as comunicacoes " << timem << "s\n";
    	}

    	else if(myrank == 1){
      		PrimeServer *ps2 = new PrimeServer();

      		ps2 -> initFilter(101, 415, SMAXP);

      		for(int i = 0; i < 10; i++) {
      			timeim = MPI_Wtime();
				MPI_Recv(ar, pack/2, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, &status);
          		timefm = MPI_Wtime();
          		timem += (timefm-timeim)/1000;

          		timei = MPI_Wtime();
          		ps2->mprocess(ar,pack/2);
          		timef = MPI_Wtime();
          		time += (timef-timei)/1000;

				timeim = MPI_Wtime();
			    MPI_Send(ar, pack/2, MPI_INT, myrank+1, 0, MPI_COMM_WORLD);
          		timefm = MPI_Wtime();
          		timem += (timefm-timeim)/1000;
      		}
          	
        	std::cout << "O process 1 demorou " << time << "s e as comunicacoes " << timem << "s\n";
    	}

    	else if(myrank == 2){
      		PrimeServer *ps3 = new PrimeServer();

      		ps3 -> initFilter(416, SMAXP, SMAXP);

      		for(int i = 0; i < 10; i++) {
      			timeim = MPI_Wtime();
				MPI_Recv(ar, pack/2, MPI_INT, myrank-1, 0, MPI_COMM_WORLD, &status);
          		timefm = MPI_Wtime();
          		timem += (timefm-timeim)/1000;

          		timei = MPI_Wtime();
          		ps3->mprocess(ar,pack/2);
          		timef = MPI_Wtime();
          		time += (timef-timei)/1000;
      		}

        	std::cout << "O process 2 demorou " << time << "s e as comunicacoes " << timem << "s\n";

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