#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: $0 nprocs problem-size"
	exit 1
fi

nprocs=$1
problemsize=$2
qsub -cwd -o "sieve_of_erastosthenes_mpi_${nprocs}_${problemsize}.dat" -e "sieve_of_erastosthenes_mpi_${nprocs}_${problemsize}.dat" -N "sieve_of_erastosthenes_mpi" -pe openmpi-roundrobin $nprocs helper.sge $nprocs ./sieve_of_erastosthenes_mpi $problemsize
