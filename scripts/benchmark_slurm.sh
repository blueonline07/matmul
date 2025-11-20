#!/bin/bash
#SBATCH --job-name=matmul_benchmark
#SBATCH --output=benchmark_%j.out
#SBATCH --error=benchmark_%j.err
#SBATCH --time=04:00:00
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --mem-per-cpu=4G
#SBATCH --exclusive
#SBATCH --partition=compute  # Adjust for your cluster

# SLURM Job Script for Matrix Multiplication Benchmarking
# 
# Usage:
#   sbatch scripts/benchmark_slurm.sh
#
# Customize:
#   - --nodes: Number of compute nodes
#   - --ntasks-per-node: MPI processes per node
#   - --cpus-per-task: OpenMP threads per MPI process
#   - --time: Maximum job time (HH:MM:SS)
#   - --partition: Your cluster's partition name

echo "=========================================="
echo "SLURM Job Started"
echo "Job ID: $SLURM_JOB_ID"
echo "Nodes: $SLURM_JOB_NUM_NODES"
echo "Tasks per node: $SLURM_NTASKS_PER_NODE"
echo "CPUs per task: $SLURM_CPUS_PER_TASK"
echo "Total CPUs: $SLURM_CPUS_ON_NODE"
echo "=========================================="

# Load required modules (adjust for your supercomputer)
# Example for common setups:
# module purge
# module load gcc/11.2.0
# module load openmpi/4.1.0
# module load cmake/3.20.0

# Or if using system defaults, comment out module loads

# Print module versions
echo ""
echo "--- Environment ---"
echo "GCC version:"
gcc --version | head -1 || echo "GCC not found"
echo ""
echo "MPI version:"
mpirun --version | head -1 || echo "MPI not found"
echo ""
echo "OpenMP threads available: $OMP_NUM_THREADS"
echo ""

# Set working directory
cd $SLURM_SUBMIT_DIR
echo "Working directory: $(pwd)"
echo ""

# Set OpenMP environment variables
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_DISPLAY_ENV=TRUE

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
RESULTS_DIR="results/slurm_${SLURM_JOB_ID}"
mkdir -p $RESULTS_DIR

# Run correctness tests first
echo "--- Running Correctness Tests ---"
mpirun -np $SLURM_NTASKS_PER_NODE ./bin/correctness > ${RESULTS_DIR}/correctness.txt 2>&1
echo "Correctness tests complete"
echo ""

# Run performance benchmarks
echo "--- Running Performance Benchmarks ---"

# Strong scaling test
echo "Running strong scaling benchmark..."
./scripts/benchmark_strong_scaling.sh 4096

# Weak scaling test
echo "Running weak scaling benchmark..."
./scripts/benchmark_weak_scaling.sh 2048

# Comprehensive test
echo "Running comprehensive benchmark..."
./scripts/benchmark_all_implementations.sh 1024 2048 4096

# Move all results to job-specific directory
mv results/strong_scaling_* $RESULTS_DIR/ 2>/dev/null || true
mv results/weak_scaling $RESULTS_DIR/ 2>/dev/null || true
mv results/comprehensive $RESULTS_DIR/ 2>/dev/null || true

echo ""
echo "=========================================="
echo "Benchmark Complete!"
echo "Results saved to: $RESULTS_DIR"
echo "Job finished at: $(date)"
echo "=========================================="

