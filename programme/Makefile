CC=mpicc
CFLAGS=-c -Wall
SOURCES=matrix_multiplication_mpi.c sieve_of_erastosthenes_mpi.c common.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=matrix_multiplication_mpi sieve_of_erastosthenes_mpi

all: $(EXECUTABLES)

matrix_multiplication_mpi: $(OBJECTS)
	$(CC) matrix_multiplication_mpi.o common.o -o $@

sieve_of_erastosthenes_mpi: $(OBJECTS)
	$(CC) sieve_of_erastosthenes_mpi.o common.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES) $(OBJECTS) 
