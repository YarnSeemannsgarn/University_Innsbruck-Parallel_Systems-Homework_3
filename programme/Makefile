CC=mpicc
CFLAGS=-c -Wall
SOURCES=matrix_multiplication_mpi.c sieve_of_erastosthenes_mpi.c dijkstra_mpi.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=matrix_multiplication_mpi sieve_of_erastosthenes_mpi dijkstra_mpi

all: $(EXECUTABLES)

matrix_multiplication_mpi: $(OBJECTS)
	$(CC) matrix_multiplication_mpi.o util.o -o $@

sieve_of_erastosthenes_mpi: $(OBJECTS)
	$(CC) sieve_of_erastosthenes_mpi.o util.o -o $@

dijkstra_mpi: $(OBJECTS)
	$(CC) dijkstra_mpi.o util.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES) $(OBJECTS) 
