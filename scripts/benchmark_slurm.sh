#!/bin/bash
#SBATCH --job-name=matmul_benchmark
#SBATCH --output=benchmark_%j.out
#SBATCH --error=benchmark_%j.err
#SBATCH --time=04:00:00
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=16G

# SLURM Job Script for Matrix Multiplication Benchmarking
# Customize: --nodes, --ntasks-per-node, --cpus-per-task, --time, --partition

echo "=========================================="
echo "SLURM Job: $SLURM_JOB_ID"
echo "Nodes: $SLURM_JOB_NUM_NODES"
echo "CPUs: $SLURM_CPUS_ON_NODE"
echo "=========================================="

# Load modules (adjust for your cluster)
# module load gcc openmpi

cd $SLURM_SUBMIT_DIR
make clean && make

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Run benchmarks
./scripts/benchmark.sh all 1024 2048 4096

echo "Benchmark complete!"
