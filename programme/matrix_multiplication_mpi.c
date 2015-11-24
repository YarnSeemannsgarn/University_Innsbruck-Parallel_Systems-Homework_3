#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>

#include "util.h"

int *matrix_multiplication(int *matrix1, size_t rows1, size_t columns1, int *matrix2, size_t rows2, size_t columns2){
  int *result = create_matrix(rows1, columns2);
  int sum = 0;
  int i, j, k;
  for(i=0; i<rows1; ++i){
    for(j=0; j<columns2; ++j){
      for(k=0; k<rows2; ++k){
	sum = sum + matrix1[i*columns1+k] * matrix2[k*columns2+j];
      }
      result[i*columns1+j] = sum;
      sum = 0;
    }
  }

  return result;
}

int main(int argc, char *argv[]){
  MPI_Init(&argc, &argv);
  
  int rank, comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  
  // Handle parameter
  if(argc != 2 && argc != 3){
    fprintf(stderr, USAGE_MESSAGE, argv[0]);
    return EXIT_FAILURE;
  }
  size_t N = atoi(argv[1]);
  int send_count = N * N / comm_size;

  int *matrix1;
  int *matrix2;
  int *result_matrix;
  clock_t begin, end;
  double time_spent;

  matrix2 = create_matrix(N, N);   // Everybody needs the second matrix
  if(rank == 0){
    // Root process creates first and result matrix
    matrix1 = create_matrix(N, N);
    result_matrix = create_matrix(N, N);

    // Initialise matrices
    int i;
    srand48(time(NULL));
    for(i=0; i<N*N; ++i){
      matrix1[i] = lrand48() % RANGE_MAX;
      matrix2[i] = lrand48() % RANGE_MAX;
    }

    // Print Matrix for matrix multiplication
    if(argc == 3){
      printf("Matrices for matrix multiplication:\n");
      print_matrix(matrix1, N, N);
      print_matrix(matrix2, N, N);
      printf("Comm_size: %i\n", comm_size);
      printf("Send_count: %i\n\n", send_count);
    }

    // Measure time at root
    begin = clock();
  }

  // Scatter matrix1 and broadcast matrix2
  int *scatter_receive = malloc(send_count * sizeof(int));
  MPI_Scatter(matrix1, send_count, MPI_INT, scatter_receive, send_count, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(matrix2, N*N, MPI_INT, 0, MPI_COMM_WORLD);

  // Mutlitply the matrix partitions (consider your rank)
  int rows_per_process = send_count / N;
  int *result_part = matrix_multiplication(scatter_receive, rows_per_process, N, matrix2, N, N);

  // Gather results
  MPI_Gather(result_part, send_count, MPI_INT, result_matrix, send_count, MPI_INT, 0, MPI_COMM_WORLD);

  // Print result And give time
  if(rank == 0) {
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %fs\n", time_spent);
    if(argc == 3) {
      printf("\nResult matrix:\n");
      print_matrix(result_matrix, N, N);
    
    }
    free(matrix1);
    free(result_matrix);
  }

  // Free memory
  free(matrix2);
  free(scatter_receive);
  free(result_part);
  
  MPI_Finalize();
  return EXIT_SUCCESS;
}
