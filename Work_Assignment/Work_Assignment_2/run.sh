#!/bin/sh

#PBS -l walltime=1:00:00
#PBS -l nodes=2:r641:ppn=32
#PBS -q mei

cd /home/a85731/CPD/PCP/Trabalhos/TP2

module load gcc/5.3.0
module load gnu/openmpi_eth/1.8.4

make clean
make

for size in 65536 524288 4194304 33554432
do
	echo "Tamanho: $size" >> ./Results/results_seq.txt
	echo "Tamanho: $size" >> ./Results/results_mpi_ppr1core.txt
	echo "Tamanho: $size" >> ./Results/results_mpi_node.txt

	for thread in 2 4 8 16 32
	do
		echo "  N de buckets: $thread" >> ./Results/results_seq.txt
		echo "  N de buckets: $thread" >> ./Results/results_mpi_ppr1core.txt
		echo "  N de buckets: $thread" >> ./Results/results_mpi_node.txt

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			./bucket_sort.seq $size $thread >> ./Results/results_seq.txt
		done

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			mpirun --map-by ppr:1:core -np $thread  -report-bindings --mca btl self,sm,tcp ./bucket_sort.mpi $size >> ./Results/results_mpi_ppr1core.txt
		done

		for i in 0 1 2 3 4 5 6 7 8 9
		do
			mpirun --map-by node -np $thread  -report-bindings --mca btl self,sm,tcp ./bucket_sort.mpi $size >> ./Results/results_mpi_node.txt
		done
	done
	
	echo "" >> ./Results/results_seq.txt
	echo "" >> ./Results/results_mpi_ppr1core.txt
	echo "" >> ./Results/results_mpi_node.txt
done
