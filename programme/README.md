How to run the programme/s
==========================

First of all build all programmes:

    $ make
    
Afterwards you can run the programme (e.g. matrix_multiplication ) with mpirun as follows (do not forget to load the openmpi module):

    $ mpirun -np <processes> ./matrix_multiplication <problem-size>
    
If you want to submit a job (e.g. sieve_of_erastothenes) you have to run the appropriate script:

    $ ./sieve_of_erastothenes <processes> <problem-size>
    
If you want to clean up just do:

    $ make clean
    
Measurements (Task 5)
=====================

Host machine: LCC Cluster with 5 nodes, where each node contains 8 CPUs
Measurement technique: clock c-libraries

For each programme I tested different big problem sizes. For the following I found relatively "good" efficencies.

Programme: matrix_multiplication with problem size 2000\*2000\*2

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |          91.09	|       - |         -
2      |          46.59 |    1.96 |      0.98
4      |          23.96 |    3.80 |      0.95      
8      |          12.80 |    7.11 |      0.89
16     |           6.99 |   13.03 |      0.81
32     |           4.32 |   21.08 |      0.65

Programme: sieve_of_erastothenes with problem size 1000000000

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |         143.72 |       - |         -
2      |          78.89 |    1.82 |      0.91
4      |          38.97 |    3.69 |	 0.92
8      |          30.59 |    4.70 |      0.59
16     |          17.23 |    8.34 |	 0.52
32     |          11.93 |   12.05 |      0.38 

Programme: dijkstra with problem size 8000

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |           0.75 |       - |         -
2      |          19.40 |    0.04 |      0.02  
4      |          18.82 |    0.04 |	 0.01
8      |          17.93 |    0.04 |	 0.01
16     |          17.82 |    0.04 |	 0.01
32     |          17.96 |    0.04 |	 0.01       |

The efficencies for matrix_multiplication and sieve_of_erastothenes were much better than the dijkstra efficencies. This can have multiple reasons: implementation/measurement details, the algorithm itself and the parallelisation techniques for the algorithms.

The efficiencies for matrix_multiplication and sieve_of_erastothenes were near to 1 for 2 and 4 processes. From 8 to 32 the efficency sinks, probably due to more parallelisation overhead relative to the sequential part. The speedup of these algorithms fulfil [Amdahl's Law](https://en.wikipedia.org/wiki/Amdahl's_law).
