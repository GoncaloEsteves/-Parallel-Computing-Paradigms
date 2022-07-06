#include <stdio.h>
#include <omp.h>

double fib(double n){
	double i, j;

	if (n<2)
		return(n);

	else if (n >= 41){
		#pragma omp task shared(i) firstprivate(n)
		i = fib(n-1);
		#pragma omp task shared(j) firstprivate(n)
		j = fib(n-2);

		#pragma omp taskwait
		return (i+j);
	}

	else{
		i = fib(n-1);
		j = fib(n-2);

		return (i+j);
	}
}

int main(){
	double r;
	double time = omp_get_wtime();
	int n = 45;

	#pragma omp parallel shared(n)
	{
		#pragma omp single
		{
			r = fib(n);

			printf("F=%.0f Time=%f\n", r, omp_get_wtime()-time);
		} 
	}

	return 1;
}