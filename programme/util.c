#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "util.h"

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

matrix_t matrix_create(const unsigned int rows, const unsigned int cols) {
  matrix_value_t * memory = calloc(rows * cols, sizeof(matrix_value_t));
  if (memory == NULL) {
    fprintf(stderr, "could not allocate memory for data\n");
    return NULL;
  }

  matrix_t matrix = calloc(rows, sizeof(matrix_value_t *));
  if (matrix == NULL) {
    fprintf(stderr, "could not allocate memory for data access helper\n");
    free(memory);
    return NULL;
  }
  int i;
  for(i=0; i<rows; i++) {
    matrix[i] = memory + i*cols;
  }

  return matrix;
}

void matrix_delete(matrix_t matrix) {
  free(matrix[0]);
}

void matrix_init(matrix_t matrix, const unsigned int rows, const unsigned int cols) {
  int i, j;
  for(i=0; i<rows; i++) {
    for(j=0; j<cols; j++) {
      matrix[i][j] = rand();
    }
  }
}

void matrix_print(const matrix_t matrix, const unsigned int rows, const unsigned int cols) {
  int i, j;
  for(i=0; i<rows; i++) {
    for(j=0; j<cols; j++) {
      printf("%f\t", matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

array_t array_create(const unsigned int size) {
  array_t array = calloc(size, sizeof(array_value_t));
  if (array == NULL) {
    fprintf(stderr, "could not allocate memory for data\n");
    return NULL;
  }
  return array;
}

void array_init(array_t array, const unsigned int size) {
  int i;
  for(i=0; i<size; i++) {
    array[i] = rand();
  }
}

void array_delete(array_t array) {
  free(array);
}

inline void array_swap(array_t array, const unsigned int i, const unsigned int j) {
  const array_value_t tmp = array[i];
  array[i] = array[j];
  array[j] = tmp;
}

int array_check_sorted(const array_t array, const unsigned int size) {
  unsigned int i;
  for(i=1; i<size; i++) {
    if(array[i] < array[i-1]) {
      fprintf(stderr, "error: array not sorted at element %d\n", i);
      return 1;
    }
  }
  return 0;
}

#define MILLISECONDS_PER_SECOND 1000UL

void timer_start(timepoint_t * time) {
  *time = MPI_Wtime();
}

unsigned long timer_end(const timepoint_t * start) {
  timepoint_t end;
  timer_start(&end);

  const unsigned long start_ms = (*start) * MILLISECONDS_PER_SECOND;
  const unsigned long end_ms = end * MILLISECONDS_PER_SECOND;
  return end_ms - start_ms;
}

void timer_print_end(const timepoint_t * start) {
  printf("elapsed time: %lums\n", timer_end(start));
}
