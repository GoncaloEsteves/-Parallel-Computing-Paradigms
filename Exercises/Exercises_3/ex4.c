#include <stdio.h>
#include <math.h>
#include <omp.h>

//nao esquecer export OMP_NUM_THREADS=8

int main(){
	int aux;
	int result[8] = {0,0,0,0,0,0,0,0};

	#pragma omp parallel for schedule(dynamic,100)
	for(int i = 0; i < 10000000; i++) {
		aux = omp_get_thread_num();
		result[aux] += sin(i);
	}
}