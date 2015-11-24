#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: $0 nprocs problem-size"
	exit 1
fi

nprocs=$1
problemsize=$2
qsub -cwd -o "matrix_multiplication_mpi_${nprocs}_${problemsize}.dat" -e "matrix_multiplication_mpi_error_${nprocs}_${problemsize}.dat" -N "matrix_multiplication_mpi" -pe openmpi-roundrobin $nprocs helper.sge $nprocs ./matrix_multiplication_mpi $problemsize
