#include <omp.h>
#include <stdio.h>

#define size 100000
double a[size], b[size];

void main() {
	// init
	for(int i=0;i<size; i++) {
		a[i] = 1.0/((double) (size-i));
		b[i] = a[i] * a[i];
	}

	// computedot
	double dot = 0;
	#pragma omp parallel for
	for(int i=0;i<size; i++) {
		dot += a[i]*b[i];
	}

	printf("Dot is %18.16f\n",dot);
}