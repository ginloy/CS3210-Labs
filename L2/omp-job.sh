#!/bin/bash

## This is a Slurm job script for Lab 2: mm-omp.cpp

#SBATCH --job-name=lab2-mmomp
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --nodelist=soctf-pdc-005
#SBATCH --partition=xs-4114
#SBATCH --mem=1gb
#SBATCH --time=00:10:00
#SBATCH --output=lab2_xs-4114-005_rows.slurmlog
#SBATCH --error=lab2_xs-4114-005_rows.slurmlog
#SBATCH --mail-type=NONE

# Check that two arguments were passed (matrix size and number of openmp threads)
if [ ! "$#" -eq 2 ]
then
  echo "Expecting 2 arguments (<matrix size> <max num threads>), got $#"
  exit 1
fi

echo "Running job: $SLURM_JOB_NAME!"
echo "We are running on $(hostname)"
echo "Job started at $(date)"
echo "Arguments to your executable: $@"

# Compile your code in case you forgot
echo "Compiling..."
srun g++ -g -O3 -fopenmp -o mm-omp-rows mm-omp-rows.cpp
# Runs your script with the arguments you passed in
echo "Running..."

threads=1
while [ $threads -le $2 ];
do
  for j in $(seq 1 3);
  do 
    echo THREADS: $threads
    srun perf stat -e cycles,instructions,fp_arith_inst_retired.scalar_single ./mm-omp-rows $1 $threads
  done
  threads=$(echo "$threads * 2" | bc)
done

echo "Job ended at $(date)"
