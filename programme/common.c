#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "common.h"

void swap(int *a, int *b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

// Cannot use two-dimensional array, because MPI_Scatter expects flat array
int *create_matrix (size_t M, size_t N) {
    int *matrix;
    int h;

    if (( matrix = malloc(M*N*sizeof(int))) == NULL) {
        printf("Malloc error");
        exit(1);
    }

    for (h=0; h<M*N; h++) {
        matrix[h] = h+1;
    }

    return matrix;
}

void print_matrix(int *matrix, size_t M, size_t N){
  int i, j;
  for(i=0; i<M; ++i){
    for(j=0; j<N; ++j){
      printf("%*i ", PRINTF_WIDTH, matrix[i*M+j]);
    }
    printf("\n");
  }
  printf("\n");
}
