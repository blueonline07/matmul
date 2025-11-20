#!/bin/bash
# Strong Scaling Benchmark - Fixed problem size, varying parallelism
# Tests how performance scales when increasing number of processors
# while keeping problem size constant

set -e  # Exit on error

MATRIX_SIZE=${1:-4096}  # Default: 4096, can override: ./benchmark_strong_scaling.sh 2048
OUTPUT_DIR="results/strong_scaling_${MATRIX_SIZE}"
mkdir -p $OUTPUT_DIR

echo "=========================================="
echo "Strong Scaling Benchmark"
echo "Matrix Size: ${MATRIX_SIZE}×${MATRIX_SIZE}"
echo "Output Directory: $OUTPUT_DIR"
echo "=========================================="
echo ""

# Ensure binary exists
if [ ! -f "./bin/performance" ]; then
    echo "Error: ./bin/performance not found. Run 'make' first."
    exit 1
fi

# Test OpenMP (single node)
echo "--- OpenMP Strong Scaling (Single Node) ---"
for threads in 1 2 4 8 16 32 64; do
    if [ $threads -gt $(nproc 2>/dev/null || echo 64) ]; then
        echo "Skipping $threads threads (exceeds available cores)"
        continue
    fi
    
    echo "  Testing with $threads OpenMP threads..."
    export OMP_NUM_THREADS=$threads
    export OMP_PLACES=cores
    export OMP_PROC_BIND=close
    
    ./bin/performance > ${OUTPUT_DIR}/openmp_${threads}threads.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1  # Brief pause between tests
done

# Test MPI (multi-node capable)
echo ""
echo "--- MPI Strong Scaling ---"
for procs in 1 2 4 8 16 32 64; do
    echo "  Testing with $procs MPI processes..."
    
    # Check if mpirun is available
    if ! command -v mpirun &> /dev/null; then
        echo "  Warning: mpirun not found, skipping MPI tests"
        break
    fi
    
    mpirun -np $procs ./bin/performance > ${OUTPUT_DIR}/mpi_${procs}procs.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
done

# Test Hybrid (MPI + OpenMP)
echo ""
echo "--- Hybrid Strong Scaling (MPI + OpenMP) ---"
for nodes in 1 2 4 8; do
    threads_per_node=8
    total_procs=$nodes
    
    # Check if mpirun is available
    if ! command -v mpirun &> /dev/null; then
        echo "  Warning: mpirun not found, skipping Hybrid tests"
        break
    fi
    
    echo "  Testing with $nodes MPI processes × $threads_per_node OpenMP threads = $((nodes * threads_per_node)) total parallelism..."
    export OMP_NUM_THREADS=$threads_per_node
    export OMP_PLACES=cores
    export OMP_PROC_BIND=close
    
    mpirun -np $nodes ./bin/performance > ${OUTPUT_DIR}/hybrid_${nodes}nodes_${threads_per_node}threads.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
done

echo ""
echo "=========================================="
echo "Strong Scaling Benchmark Complete!"
echo "Results saved to: $OUTPUT_DIR"
echo "=========================================="

