#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

struct estructura{

	int clave;

};

void merge(struct estructura array[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    struct estructura L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = array[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = array[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i].clave <= R[j].clave) 
        { 
            array[k] = L[i]; 
            i++; 
        } 
        else
        { 
            array[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        array[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        array[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */

void mergeSort(struct estructura arre[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arre, l, m); 
        mergeSort(arre, m+1, r); 
  
        merge(arre, l, m, r); 
    } 
} 
  
 void merge2(struct estructura arr1[], struct estructura arr2[], int size1, int size2, struct estructura arr3[]){
    int i=0;
    int j=0;
    int k;
    int apuntador=0;

    for(k=0 ; k < size1 + size2 ; k++){
        if(i<size1){
            if(j<size2){ //No se acaba ninguno
                if(arr1[i].clave < arr2[j].clave){
                    arr3[k] = arr1[i];
                    i++;
                }
                else{
                    arr3[k] = arr2[j];
                    j++;
                }
            }
            else{ // se acaba el segundo
                arr3[k] = arr1[i];
                i++;
            }
        }
        else{   //se acaba el primero
                arr3[k] = arr2[j];
                j++;
        }
    }
 }
  
/* Driver program to test above functions */
int main(int argc, char ** argcv) 
{ 	
	
	MPI_Init(&argc,&argcv);

	int rank,nproc,i,aux_size;
	double tiempo1, tiempo2,tiempo3;
	
	MPI_Status status;
	MPI_Datatype MPI_Struct;
	MPI_Type_contiguous(1,MPI_INT,&MPI_Struct);
	MPI_Type_commit(&MPI_Struct);	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&nproc);
	srand(time(NULL));
	struct estructura aux [16];
	
	struct estructura *arr;
	
	aux_size = sizeof(aux) / sizeof(aux[0]);
	arr = malloc(aux_size/nproc * sizeof(struct estructura));
	int  arr_size = aux_size/nproc;
	
	for( i = 0 ; i < aux_size ; i++){
	
		aux[i].clave =  rand() % 100000;
	
	}

	tiempo1 = MPI_Wtime();
	double tiempo7, tiempo8;
	tiempo7 = MPI_Wtime();
  	MPI_Scatter(aux,arr_size,MPI_Struct,arr,arr_size,MPI_Struct,0,MPI_COMM_WORLD);
   	tiempo8 = MPI_Wtime();
	printf("Tiempo de scatter: %f\n", tiempo8-tiempo7);
   	mergeSort(arr,0,arr_size-1);
	
	int cantCom = (int)(log (nproc)/ log (2)); 
	
	int mult =1,j;
		
	for( i = 0 ; i < cantCom ; i++){

		if((rank/mult)%2==1){
			double tiempo4 = MPI_Wtime();
			double tiempo5;
		
			MPI_Send(arr,arr_size*mult,MPI_Struct,rank-mult,0,MPI_COMM_WORLD);
			MPI_Recv(&arr, arr_size*mult, MPI_Struct, rank-mult,0,MPI_COMM_WORLD, &status);
			tiempo5 = MPI_Wtime();
			printf("Tiempo de comunicacion: %f\n", (tiempo5 - tiempo4) /2);
			MPI_Finalize();
			return 0;
		}
		else{
			
			struct estructura arr2[arr_size * mult];
			
			MPI_Recv(arr2,arr_size*mult,MPI_Struct,rank + mult, 0, MPI_COMM_WORLD,&status);
			MPI_Send(arr,arr_size*mult,MPI_Struct, rank+mult,0,MPI_COMM_WORLD);
			printf("Tiempo luego de recibir %i\n", MPI_Wtime);	
			struct estructura *arr3;
			arr3 = (struct estructura*) malloc((arr_size * 2 * mult) * sizeof(struct estructura));
			
			merge2 (arr, arr2, arr_size * mult , arr_size * mult,arr3);
			
			arr = (struct estructura*)realloc(arr, arr_size * 2 * mult * sizeof(struct estructura));
			
			for (j = 0 ; j < arr_size * 2 * mult ; j++){
				
				arr[j] = arr3[j];
				
			}
		}
		
		mult = mult *2;
	}
	tiempo2 = MPI_Wtime();
	
    printf("Arreglo ordenado : \n");
    printf("\nTiempo transcurrido : %f\n", tiempo2-tiempo1);
    for(i = 0 ; i < aux_size ; i ++){
    	printf("%i,",arr[i].clave);
	}
    
    //printf("Tiempo total : %f ",tiempo3-tiempo1);
    MPI_Finalize();
    
   	return 0;
} 
	
	
