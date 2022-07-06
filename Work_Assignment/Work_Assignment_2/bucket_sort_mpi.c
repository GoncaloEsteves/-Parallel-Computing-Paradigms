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
struct bucket * bucketSort(int* array, int size, int bsize){
    struct bucket* buckets = (struct bucket*) malloc(sizeof(struct bucket) * bsize);
    int i, j;

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

    return buckets;
}
 
int main(int argc, char *argv[]) {

    if(argc < 2 || argc > 2){
        printf("Argumentos: #ArraySize\n");
        return 1;
    }

    int bsize, id;

    //Inicializacao
    MPI_Init (&argc, &argv); 
    //Averiguacao de quantos processos ha
    MPI_Comm_size (MPI_COMM_WORLD, &bsize);
    //Averiguacao de qual o processo em que estamos
    MPI_Comm_rank (MPI_COMM_WORLD, &id); 

    if(bsize > 1){
        int size = atoi(argv[1]);
        int division = size/bsize;
        int i, pos;
        int* array;
        int* aux = (int*) malloc(sizeof(int) * size);
        int* bucAux = (int*) malloc(sizeof(int) * size);
        int* sizes = (int*) malloc(sizeof(int) * bsize);
        int* displs = (int*) malloc(sizeof(int) * bsize);
        MPI_Status status;
        double start, end, exec_time;
        double startComm, endComm, TComm = 0;
        double startComp, endComp, TComp = 0;
        struct bucket* buckets;

        //Criação do array a ordenar e iniciação do tempo
        if(id == 0){
            array = malloc(sizeof(int) * size);

            srand(time(NULL));

            //Preenchimento do array com valores aleatorios
            for(int i = 0; i < size; i++){
                array[i] = rand();
            }
            //Tempo inicial
            start = MPI_Wtime();

            //Inicio do calculo do tempo de Scatter
            startComm = MPI_Wtime();
        }

        //Divisao do array inicial por todos os processos, recebendo cada um "division" elementos, guardados em "aux"
        MPI_Scatter(array, division, MPI_INT, aux, division, MPI_INT, 0, MPI_COMM_WORLD);

        if(id == 0){
            //Fim do calculo do tempo de Scatter
            endComm = MPI_Wtime();
            TComm += (endComm - startComm) * 1000;

            //Inicio do calculo do tempo do BucketSort
            startComp = MPI_Wtime();
        }      

        //Ordenacao dos elementos de cada processo, distribuindo-os pelos buckets respetivos
        buckets = bucketSort(aux, division, bsize);

        if(id == 0){
            //Fim do calculo do tempo do BucketSort
            endComp = MPI_Wtime();
            TComp += (endComp - startComp) * 1000;

            //Inicio do calculo do tempo de Send e Recv
            startComm = MPI_Wtime();
        }

        //Criacao dos requests necessarios
        MPI_Request *requests = (MPI_Request*) malloc(sizeof(MPI_Request) * bsize);

        //Envio distribuicao dos buckets criados pelos processos que serao responsaveis por organiza-los
        for(i = 0; i < bsize; i++){
            if(i != id)
                //Utiliza-se o Isend por forma a utilizar sends nao bloqueantes
                //Para alem disso, envia-se na tag o numero de elementos de cada bucket
                MPI_Isend(buckets[i].value, buckets[i].count, MPI_INT, i, buckets[i].count, MPI_COMM_WORLD, &requests[i]);
            else{
                for(pos = 0; pos < buckets[i].count; pos++)
                    bucAux[pos] = buckets[i].value[pos]; 
            }
        }

        //Rececao dos buckets associados a cada processo, provenientes dos outros processos
        for(i = 0; i < bsize-1; i++){
            //Fazem-se bsize-1 iteracoes, pois nao e necessario receber o bucket do proprio processo
            //Utiliza-se o ANY_SOURCE, por forma a não forcar a rececao ordenada dos buckets, uma vez que nao e necessario
            //Utiliza-se o ANY_TAG, ja que o tamanho do array vem na tag
            MPI_Recv(&(bucAux[pos]), division, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            pos += status.MPI_TAG;
        }

        if(id == 0){
            //Fim do calculo do tempo de Send e Recv
            endComm = MPI_Wtime();
            TComm += (endComm - startComm) * 1000;

            //Inicio do calculo do tempo de ordenacao do bucket
            startComp = MPI_Wtime();
        }

        //Ordenacao do array com os numeros de cada processo
        mergeSort(bucAux, 0, pos);

        if(id == 0){
            //Fim do calculo do tempo de ordenacao do bucket
            endComp = MPI_Wtime();
            TComp += (endComp - startComp) * 1000;

            //Inicio do calculo do tempo de Gather
            startComm = MPI_Wtime();
        }

        //Colecao do numero de elementos que cada processo ordenou
        MPI_Gather(&pos, 1, MPI_INT, sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);

        //Calculo da posicao a partir da qual deverao ser guardados os elementos recebidos de cada processo
        if(id == 0){
            displs[0] = 0;
            for(i = 1; i < bsize+1; i++)
                displs[i] = displs[i-1] + sizes[i-1]; 
        }

        //Colecao de todos os elementos, ordenados, provenientes de cada um dos processos
        MPI_Gatherv(bucAux, pos, MPI_INT, array, sizes, displs, MPI_INT, 0, MPI_COMM_WORLD);

        if(id == 0){
            //Fim do calculo do tempo de Gather
            endComm = MPI_Wtime();
            TComm += (endComm - startComm) * 1000;

            //Tempo final
            end = MPI_Wtime();
            //Determinacao do tempo demorado a ordenar o array
            exec_time = (end - start) * 1000;
            printf("      Tempo de execucao: %gms\n", exec_time);
            printf("      Tempo de comunicacao: %gms\n", TComm);
            printf("      Tempo de computacao: %gms\n", TComp);

            //Verificar se o array final está ordenado
            /*if(isOrdered(array, size) == 1)
                printf("      Array ordenado.\n");
            else
                printf("      Array desordenado.\n");*/

            free(array);
        }
    }

    MPI_Finalize();
 
    return 0;
}