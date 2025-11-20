#!/bin/bash
#PBS -N matmul_benchmark
#PBS -o benchmark_${PBS_JOBID}.out
#PBS -e benchmark_${PBS_JOBID}.err
#PBS -l walltime=04:00:00
#PBS -l nodes=8:ppn=8
#PBS -l mem=32gb
#PBS -q compute  # Adjust for your cluster

# PBS/Torque Job Script for Matrix Multiplication Benchmarking
#
# Usage:
#   qsub scripts/benchmark_pbs.sh
#
# Customize:
#   - nodes=X:ppn=Y: Number of nodes and processors per node
#   - walltime: Maximum job time (HH:MM:SS)
#   - q: Queue/partition name

echo "=========================================="
echo "PBS Job Started"
echo "Job ID: $PBS_JOBID"
echo "Nodes: $PBS_NODEFILE"
echo "=========================================="

# Load required modules (adjust for your supercomputer)
# module load gcc/11.2.0
# module load openmpi/4.1.0

# Set working directory
cd $PBS_O_WORKDIR
echo "Working directory: $(pwd)"
echo ""

# Count available processors
NUM_NODES=$(cat $PBS_NODEFILE | sort -u | wc -l)
PPN=$(cat $PBS_NODEFILE | grep $(head -1 $PBS_NODEFILE) | wc -l)
TOTAL_PROCS=$((NUM_NODES * PPN))

echo "Nodes: $NUM_NODES"
echo "Processors per node: $PPN"
echo "Total processors: $TOTAL_PROCS"
echo ""

# Set OpenMP environment
export OMP_NUM_THREADS=$PPN
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Build the project
echo "--- Building Project ---"
make clean
make
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi
echo "Build successful!"
echo ""

# Create results directory
RESULTS_DIR="results/pbs_${PBS_JOBID}"
mkdir -p $RESULTS_DIR

# Run benchmarks
echo "--- Running Benchmarks ---"

# Use mpirun with node file
mpirun -np $TOTAL_PROCS -machinefile $PBS_NODEFILE ./bin/performance > ${RESULTS_DIR}/performance.txt 2>&1

echo ""
echo "=========================================="
echo "Benchmark Complete!"
echo "Results saved to: $RESULTS_DIR"
echo "=========================================="

