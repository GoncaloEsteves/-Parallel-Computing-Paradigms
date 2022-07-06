#include <stdio.h>
#include <math.h>
#include <omp.h>

int main(){
	int result[1];
	result[0] = 0;

	#pragma omp parallel for schedule(dynamic, 1)
	for(int i = 0; i < 10000000; i++) {
		result[0] += sin(i);
	}
}