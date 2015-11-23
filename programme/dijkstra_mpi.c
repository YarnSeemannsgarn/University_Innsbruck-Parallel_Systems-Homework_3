// Authors: Sebastian Sams & Jan Schlenker
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include <mpi.h>

#include "util.h"

void dijkstra_sequential(const matrix_t graph, const unsigned int vertex_count, const unsigned int source, matrix_value_t distance[]);
void dijkstra_parallel(const matrix_t graph, const unsigned int vertex_count, const unsigned int nodes_per_process, const unsigned int first_node, const unsigned int source, matrix_value_t distance_partition[]);

// searches for the index of the vertex in the set that has the least distance value
// returns the index, actual distance to the vertex is stored in min (pass NULL to ignore the parameter)
unsigned int min_distance(const matrix_value_t distance[], const bool vertex_set[], const unsigned int vertex_count, matrix_value_t * const min);

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  if (argc != 2) {
    fprintf(stderr, "usage: %s node_count\n", argv[0]);
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  const unsigned long node_count = strtoul(argv[1], NULL, 10);
  if (node_count == 0 || node_count > INT_MAX) {
    fprintf(stderr, "invalid node count\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  
  int mpi_nproc, mpi_rank;
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_nproc);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

  if (mpi_rank == 0 && node_count % mpi_nproc != 0) {
    printf("warning: node count must be a multiple of the process count for correct results!\n");
  }
  const unsigned int nodes_per_process = node_count / mpi_nproc;
  const unsigned int first_node = nodes_per_process * mpi_rank;

  // allocate memory for graph
  matrix_t graph = matrix_create(node_count, node_count);
  if (graph == NULL) {
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  
  matrix_value_t* distance = NULL;
  unsigned int source;

  if (mpi_rank == 0) {
    // initialize with random data
    srand(time(NULL));
    matrix_init(graph, node_count, node_count);
    source = rand() % node_count;

    // reserve space for result
    distance = calloc(node_count, sizeof(matrix_value_t));
    if (distance == NULL) {
      fprintf(stderr, "could not allocate memory for distance data\n");
      MPI_Finalize();
      return EXIT_FAILURE;
    }
  }
  
  // allocate memory for the distance of the local vertices
  matrix_value_t* distance_partition = calloc(nodes_per_process, sizeof(matrix_value_t));
  if (distance_partition == NULL) {
    fprintf(stderr, "could not allocate memory for partial distance data\n");
    MPI_Finalize();
    return EXIT_FAILURE;
  }
  
  timepoint_t start;
  if (mpi_rank == 0) {
    timer_start(&start);
  }
  
  // distribute data
  MPI_Bcast(graph[0], node_count * node_count, MATRIX_VALUE_MPI_TYPE, 0, MPI_COMM_WORLD);
  MPI_Bcast(&source, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
  
  // calculate
  dijkstra_parallel(graph, node_count, nodes_per_process, first_node, source, distance_partition);
  
  // collect results
  MPI_Gather(distance_partition, nodes_per_process, MATRIX_VALUE_MPI_TYPE, distance, nodes_per_process, MATRIX_VALUE_MPI_TYPE, 0, MPI_COMM_WORLD);
  
  if (mpi_rank == 0) {
    timer_print_end(&start);
  }
  
  free(distance_partition);
  
#ifdef VERIFY_ALGORITHM
  if (mpi_rank == 0) {
    printf("verifying calculation...\n");
    matrix_value_t* sequential_distance = calloc(node_count, sizeof(matrix_value_t));
    if (sequential_distance == NULL) {
      fprintf(stderr, "could not allocate memory for sequential distance data\n");
      MPI_Finalize();
      return EXIT_FAILURE;
    }
    
    dijkstra_sequential(graph, node_count, source, sequential_distance);
    for(unsigned int i=0; i < node_count; i++) {
      if (sequential_distance[i] != distance[i]) {
	fprintf(stderr, "error: wrong distance for node %d, expected %f but is %f\n", i, sequential_distance[i], distance[i]);
      }
    }
    
    printf("verification done.\n");
    free(sequential_distance);
  }
#endif // VERIFY_ALGORITHM
  
  
  // free memory
  matrix_delete(graph);
  if (mpi_rank == 0) {
    free(distance);
  }
  
  MPI_Finalize();
  return EXIT_SUCCESS;
}

void dijkstra_sequential(const matrix_t graph, const unsigned int vertex_count, const unsigned int source, matrix_value_t distance[]) {
  bool vertex_set[vertex_count];
  
  // init
  unsigned int v;
  for(v=0; v < vertex_count; v++) {
    // unknown distance
    distance[v] = MATRIX_VALUE_MAX;
    // add node to set (all nodes initially unvisited)
    vertex_set[v] = true;
  }
  // distance from source to source is zero
  distance[source] = 0;
  
  // for each vertex in the set
  unsigned int i;
  for(i=0; i < vertex_count; i++) {
    // pick vertex with minimum distance, then remove it from the set
    const unsigned int u = min_distance(distance, vertex_set, vertex_count, NULL);
    vertex_set[u] = false;
    
    // update neighbors of the picked vertex
    unsigned int v;
    for(v=0; v < vertex_count; v++) {
      if(vertex_set[v] && distance[u] + graph[u][v] < distance[v]) {
	// shorter path
	distance[v] = distance[u] + graph[u][v];
      }
    }
  }
}

void dijkstra_parallel(const matrix_t graph, const unsigned int vertex_count, const unsigned int nodes_per_process, const unsigned int first_node, const unsigned int source, matrix_value_t distance_partition[]) {
  bool vertex_set[nodes_per_process];
  const unsigned int last_node = first_node + nodes_per_process;
  
  // init
  unsigned int v;
  for(v=0; v < nodes_per_process; v++) {
    // unknown distance
    distance_partition[v] = MATRIX_VALUE_MAX;
    // add node to set (all nodes initially unvisited)
    vertex_set[v] = true;
  }
  // distance from source to source is zero
  if (source >= first_node && source < last_node) {
    distance_partition[source - first_node] = 0;
  }
  
  // for each vertex in the set
  unsigned int i;
  for(i=0; i < vertex_count; i++) {
    struct {
      matrix_value_t distance;
      unsigned int vertex;
    } local_min, global_min;
    
    // pick local vertex with minimum distance
    local_min.vertex = min_distance(distance_partition, vertex_set, nodes_per_process, &(local_min.distance));
    local_min.vertex += first_node;
    // get global minimum
    MPI_Allreduce(&local_min, &global_min, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);
    // mark vertex as visited
    if (global_min.vertex >= first_node && global_min.vertex < last_node) {
      vertex_set[global_min.vertex - first_node] = false;
    }
    
    // update neighbors of the picked vertex
    unsigned int v;
    for(v=0; v < nodes_per_process; v++) {
      if(vertex_set[v] && global_min.distance + graph[global_min.vertex][v+first_node] < distance_partition[v]) {
	// shorter path has been found
	distance_partition[v] = global_min.distance + graph[global_min.vertex][v+first_node];
      }
    }
  }
}

unsigned int min_distance(const matrix_value_t distance[], const bool vertex_set[], const unsigned int vertex_count, matrix_value_t * const min_return) {
  matrix_value_t min = MATRIX_VALUE_MAX;
  unsigned int vertex;

  unsigned int u;
  for(u = 0; u < vertex_count; u++) {
    if(vertex_set[u] && distance[u] < min) {
      min = distance[u];
      vertex = u;
    }
  }
  
  if (min_return != NULL) {
    *min_return = min;
  }
  return vertex;
}
