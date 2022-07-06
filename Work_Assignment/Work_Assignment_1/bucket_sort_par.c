#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>
 
struct bucket {
    int count;
    int* value;
};

//Funcao que compara dois inteiros, utilizada no qsort
int compareIntegers(const void* first, const void* second){
    int x = *((int*) first), y = *((int*) second);

    if (x == y){
        return 0;
    }

    else if (x < y){
        return -1;
    }
    
    else{
        return 1;
    }
}

//Funcao que verifica se um array de tamanho size esta ordenado
int isOrdered(int* array, int size){
    int i;
	for (i = 0; i < size-1; i++){
		if(array[i] > array[i+1])
			return 0;
	}

    //printf("      %d\n", i);

	return 1;
}

//Funcao de ordenacao paralela de arrays inspirada no algoritmo Bucket Sort
void bucketSort(int* array, int size){
	int bsize;

	//determinacao do numero de buckets, tendo em conta o numero de threads existentes
	#pragma omp parallel
	{
		#pragma omp single
		bsize = omp_get_num_threads();
	}

    struct bucket buckets[bsize];

    //inicio da paralelizacao, onde se permite que cada thread opere sobre um unico bucket
    #pragma omp parallel num_threads(bsize)
    {
    	//obtencao do identificador da thread,
    	//que serve tambem para identificar o bucket
    	int id = omp_get_thread_num();

    	//alocar espaco para o respetivo bucket
    	buckets[id].count = 0;
        buckets[id].value = (int*) malloc(sizeof(int) * size);

        int i;
    
    	//verificacao de quais os elementos do array que pertencem ao bucket
    	for (i = 0; i < size; i++){
        	if(
            	//Para inserir no primeiro bucket, o valor deverá ser inferior a RAND_MAX/bsize
            	((id == 0) && (array[i] <= (RAND_MAX/bsize)))
            	//Para inserir no ultimo bucket, o valor deverá ser superior a (bsize-1)*(RAND_MAX/bsize)
            	|| ((id == bsize-1) && (array[i] > id*(RAND_MAX/bsize)))
            	//Para inserir noutro qualquer bucket, o valor não devera ser inferior
            	//ao limite máximo do bucket anterior nem superior ao limite minimo do proximo bucket
            	|| ((array[i] > id*(RAND_MAX/bsize)) && (array[i] <= (id+1)*(RAND_MAX/bsize))))
        		{

                	buckets[id].value[buckets[id].count++] = array[i];
            	}
    	}

        //ordenacao do bucket, utilizando a uma funcao de ordenacao auxiliar
        qsort(buckets[id].value, buckets[id].count, sizeof(int), &compareIntegers);

        //barreira necessaria para garantir que, daqui em diante, todos os buckets estao
        //ordenados e preenchidos com todos os elementos que lhes pertencem
    	#pragma omp barrier

        //determinacao da posicao a partir da qual devem ser inseridos os elementos do bucket no array final
        int aux = 0;

        for(i = 0; i < id; i++)
            aux += buckets[i].count;

        //reinsercao dos elementos, por ordem crescente
        for (i = 0; i < buckets[id].count; i++){
            array[aux + i] = buckets[id].value[i];
        }

        free(buckets[id].value);
    }
}
 
int main(int argc, char *argv[]) {

	if(argc < 2 || argc > 2){
		printf("Argumentos: #ArraySize\n");
		return 1;
	}

    int size = atoi(argv[1]);
    int* array = malloc(sizeof(int) * size);

	srand(time(NULL));

    //preenchimento do array com valores aleatorios
    for(int i = 0; i < size; i++){
    	array[i] = rand();
    }

	double start = omp_get_wtime();

    //ordenacao recorrendo ao Bucket Sort
    bucketSort(array, size);

    double end = omp_get_wtime();
	double exec_time = (end - start) * 1000;
	printf("      %gms\n", exec_time);

    //verificar se o array final está ordenado
    /*if(isOrdered(array, size) == 1)
        printf("      Array ordenado.\n");
    else
        printf("      Array desordenado.\n");*/

    free(array);
 
    return 0;
}