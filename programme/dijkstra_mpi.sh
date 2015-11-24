#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: $0 nprocs problem-size"
	exit 1
fi

nprocs=$1
problemsize=$2
qsub -cwd -o "dijkstra_mpi_${nprocs}_${problemsize}.dat" -e "dijkstra_mpi_error_${nprocs}_${problemsize}.dat" -N "dijkstra_mpi" -pe openmpi-roundrobin $nprocs helper.sge $nprocs ./dijkstra_mpi $problemsize
