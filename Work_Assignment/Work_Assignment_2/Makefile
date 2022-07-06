CC=mpicc

all: bucket_sort.mpi bucket_sort.seq

bucket_sort.mpi: bucket_sort_mpi.c
	$(CC) -o bucket_sort.mpi -O3 -Wall -std=gnu99 -fopenmp -lm bucket_sort_mpi.c

bucket_sort.seq: bucket_sort_seq.c
	$(CC) -o bucket_sort.seq -O3 -Wall -std=gnu99 -fopenmp -lm bucket_sort_seq.c

.PHONY: clean

clean:
		rm -f bucket_sort.*