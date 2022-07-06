#!/bin/sh

#PBS -q cpar
#PBS -l nodes=1
#PBS -l walltime=1:00:00

cd /home/a85731/CPD/PCP/Trabalhos/TP1

module load gcc/5.3.0

for size in 65536 524288 4194304 33554432
do
	echo "Tamanho: $size"

	for thread in 2 4 8 16 32 48 64
	do
		echo "  N de buckets: $thread"
		echo "    Versao Sequencial"

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			./bucket_sort.seq $size $thread
		done

		echo "    Versao Paralela"

		export OMP_NUM_THREADS=$thread

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			./bucket_sort.par $size
		done
	done
	echo ""
done