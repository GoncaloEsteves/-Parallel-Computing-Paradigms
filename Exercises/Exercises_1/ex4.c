#include <omp.h>
#include <stdio.h>

int main(){
	#pragma omp parallel num_threads(2)
	//#pragma omp for schedule(static)
	//#pragma omp for schedule(static, 10)
	//#pragma omp for schedule(dynamic)
	//#pragma omp for schedule(dynamic, 10)
	#pragma omp for schedule(guided)
	for(int i = 0; i < 100; i++){
		int id = omp_get_thread_num();
		printf("T%d:i%d ", id, i);
	}
	printf("\n");
}