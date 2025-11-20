#!/bin/bash
# Weak Scaling Benchmark - Problem size grows with parallelism
# Tests how performance scales when problem size increases proportionally
# with number of processors

set -e  # Exit on error

BASE_SIZE=${1:-2048}  # Base size per process, default: 2048
OUTPUT_DIR="results/weak_scaling"
mkdir -p $OUTPUT_DIR

echo "=========================================="
echo "Weak Scaling Benchmark"
echo "Base Size per Process: ${BASE_SIZE}×${BASE_SIZE}"
echo "Output Directory: $OUTPUT_DIR"
echo "=========================================="
echo ""

# Ensure binary exists
if [ ! -f "./bin/performance" ]; then
    echo "Error: ./bin/performance not found. Run 'make' first."
    exit 1
fi

# Test MPI weak scaling
echo "--- MPI Weak Scaling ---"
for procs in 1 2 4 8 16 32; do
    size=$((BASE_SIZE * procs))
    
    # Check if mpirun is available
    if ! command -v mpirun &> /dev/null; then
        echo "  Warning: mpirun not found, skipping MPI tests"
        break
    fi
    
    echo "  Testing $procs processes with ${size}×${size} matrix (${BASE_SIZE}×${BASE_SIZE} per process)..."
    mpirun -np $procs ./bin/performance > ${OUTPUT_DIR}/mpi_${procs}procs_${size}size.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
done

# Test Hybrid weak scaling
echo ""
echo "--- Hybrid Weak Scaling (MPI + OpenMP) ---"
for nodes in 1 2 4 8; do
    threads_per_node=8
    total_procs=$((nodes * threads_per_node))
    size=$((BASE_SIZE * total_procs))
    
    # Check if mpirun is available
    if ! command -v mpirun &> /dev/null; then
        echo "  Warning: mpirun not found, skipping Hybrid tests"
        break
    fi
    
    echo "  Testing $nodes nodes × $threads_per_node threads = $total_procs total with ${size}×${size} matrix..."
    export OMP_NUM_THREADS=$threads_per_node
    export OMP_PLACES=cores
    export OMP_PROC_BIND=close
    
    mpirun -np $nodes ./bin/performance > ${OUTPUT_DIR}/hybrid_${nodes}nodes_${size}size.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
done

echo ""
echo "=========================================="
echo "Weak Scaling Benchmark Complete!"
echo "Results saved to: $OUTPUT_DIR"
echo "=========================================="

