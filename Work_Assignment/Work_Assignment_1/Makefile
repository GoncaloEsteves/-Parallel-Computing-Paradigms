CC=gcc

all: bucket_sort.par bucket_sort.seq

bucket_sort.par: bucket_sort_par.c
	$(CC) -o bucket_sort.par -O3 -Wall -std=c99 -fopenmp -lm bucket_sort_par.c

bucket_sort.seq: bucket_sort_seq.c
	$(CC) -o bucket_sort.seq -O3 -Wall -std=c99 -fopenmp -lm bucket_sort_seq.c

.PHONY: clean

clean:
		rm -f bucket_sort.*