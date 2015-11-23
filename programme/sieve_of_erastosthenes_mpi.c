#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include "mpi.h"

#include "util.h"

int main(int argc, char *argv[]){
  // MPI init
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

  // All processes compute 1 - sqrt(N)
  int square_root = (int) sqrt((double)N);
  int *shared_primes = malloc((square_root + 1) * sizeof(int));
  
  // Initialise
  shared_primes[0] = 0;
  shared_primes[1] = 0;
  int i;
  for(i=2; i<=square_root; ++i){
    shared_primes[i] = 1;
  }

  // Calculate end for each process
  int remaining = N - square_root;
  int numbers_per_process = remaining / comm_size;  
  int start = square_root + 1 + rank * numbers_per_process;
  int end = start + numbers_per_process - 1;
  int *partitioned_primes = malloc(numbers_per_process * sizeof(int));
  for(i=0; i<numbers_per_process; ++i) {
    partitioned_primes[i] = 1;
  }

  // Sieve
  for(i = 2; i <= square_root; i++) {
    if(shared_primes[i]) {
      // check numbers in shared primes
      int j;
      for(j=i*2; j<=square_root; j+=i) {
	shared_primes[j] = 0;
      }
      // check in partitioned primes
      j = i;
      while(j < start){
	j += i;
      }
      for(; j<=end; j+=i) {
	partitioned_primes[j-start] = 0;
      }
    }
  }

  // Check
  if(argc == 3) {
    if(rank == 0) {
      for(i=0; i <= square_root; ++i) {
	printf("%i: %i\n", i, shared_primes[i]); 
      } 
    }
    for(i=0; i<numbers_per_process; ++i) {
      printf("%i: %i\n", i+start, partitioned_primes[i]);
    }
  }

  // Free memory
  free(shared_primes);
  free(partitioned_primes);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
