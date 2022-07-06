#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
 
struct bucket {
    int count;
    int* value;
};

//Funcao que verifica se um array de tamanho size esta ordenado
int isOrdered(int* array, int size){
    int i;
    for (i = 0; i < size-1; i++){
        if(array[i] > array[i+1])
            return 0;
    }

    printf("      %d\n", i);

    return 1;
}

//Funcao que ordena um array, considerando que as duas metades dele já estao ordenadas
void merge(int* array, int pos, int aux, int size){

    int n1 = aux - pos;
    int n2 = size - aux;
    int i, j, k;
    int L[n1], M[n2];

    //insercao dos elementos num array auxiliar, metade em cada um
    for (i = 0; i < n1; i++)
        L[i] = array[pos + i];

    for (j = 0; j < n2; j++)
        M[j] = array[aux + j];

    //reinsercao dos elementos no array principal de forma ordenada
    for(i = 0, j = 0, k = pos; i < n1 && j < n2; k++) {
        if (L[i] <= M[j]) {
            array[k] = L[i];
            i++;
        }
        else {
            array[k] = M[j];
            j++;
        }
    }

    while (i < n1) {
        array[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        array[k] = M[j];
        j++;
        k++;
    }
}

//Funcao de ordenacao de arrays inspirada no algoritmo Merge Sort
void mergeSort(int* array, int pos, int size){
    if(pos+1 < size){
        int aux = (pos+size)/2;
        mergeSort(array, pos, aux);
        mergeSort(array, aux, size);
        merge(array, pos, aux, size);
    }
}

//Funcao de ordenacao de arrays inspirada no algoritmo Bucket Sort
void bucketSort(int* array, int size, int bsize){
    struct bucket buckets[bsize];
    int i, j, k;

    //alocar espaco para os buckets
    for (i = 0; i < bsize; i++){
        buckets[i].count = 0;
        buckets[i].value = (int*) malloc(sizeof(int) * size);
    }
    
    //insercao dos elementos do array no respetivo bucket
    for (i = 0; i < size; i++){
        //determinacao de qual o bucket onde deverá ser inserido o valor
        j = ((double)array[i]/(double)RAND_MAX) * bsize;
        //insercao no respetivo bucket
        buckets[j].value[buckets[j].count++] = array[i];
    }

    for (k = 0, i = 0; i < bsize; i++){
        //ordenacao do bucket, utilizando a funcao mergeSort
        mergeSort(buckets[i].value, 0, buckets[i].count);

        //insercao dos elementos de forma ordenada
        for (j = 0; j < buckets[i].count; j++){
            array[k + j] = buckets[i].value[j];
        }

        k += buckets[i].count;

        free(buckets[i].value);
    }
}
 
int main(int argc, char *argv[]) {

	if(argc < 3 || argc > 3){
		printf("Argumentos: #ArraySize #NumberOfBuckets\n");
		return 1;
	}

    int size = atoi(argv[1]);
    int bsize = atoi(argv[2]);
    int* array = malloc(sizeof(int) * size);

	srand(time(NULL));

    //preenchimento do array com valores aleatorios
    for(int i = 0; i < size; i++){
    	array[i] = rand();
    }

    MPI_Init (&argc, &argv);

	double start = MPI_Wtime();

    //ordenacao recorrendo ao BucketSort
    bucketSort(array, size, bsize);

    double end = MPI_Wtime();

    MPI_Finalize ();

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