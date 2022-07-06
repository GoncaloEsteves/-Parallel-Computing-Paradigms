#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(){
	int result[1];
	result[0] = 0;

	#pragma omp parallel for schedule(dynamic, 10)
	for(int i = 0; i < 10000000; i++) {
		//#pragma omp atomic
		#pragma omp critical
		result[0] += sin(i);
	}
}