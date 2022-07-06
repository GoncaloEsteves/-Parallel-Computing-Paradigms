#!/bin/sh

#PBS -l walltime=1:00:00
#PBS -l nodes=2:r641:ppn=32
#PBS -q mei

cd /home/a85731/CPD/PCP/Trabalhos/TP1

module load gcc/5.3.0

make clean
make

for size in 65536 524288 4194304 33554432
do
	echo "Tamanho: $size" >> results_omp.txt

	for thread in 2 4 8 16 32
	do
	    echo "  Nº de buckets: $thread" >> results_omp.txt

		export OMP_NUM_THREADS=$thread

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			./bucket_sort.par $size >> results_omp.txt
		done
	done
	echo "" >> results_omp.txt
done