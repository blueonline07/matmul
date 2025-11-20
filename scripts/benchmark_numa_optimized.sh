#!/bin/bash
# NUMA-Optimized Benchmark Script
# Specifically designed for 2-socket, 8-core systems with NUMA
# Optimizes for NUMA topology: 2 NUMA nodes, 4 cores each

set -e

MATRIX_SIZE=${1:-4096}  # Default: 4096
OUTPUT_DIR="results/numa_optimized_${MATRIX_SIZE}"
mkdir -p $OUTPUT_DIR

echo "=========================================="
echo "NUMA-Optimized Benchmark"
echo "System: 2 NUMA nodes, 8 cores total"
echo "Matrix Size: ${MATRIX_SIZE}×${MATRIX_SIZE}"
echo "Output Directory: $OUTPUT_DIR"
echo "=========================================="
echo ""

# Ensure binary exists
if [ ! -f "./bin/performance" ]; then
    echo "Error: ./bin/performance not found. Run 'make' first."
    exit 1
fi

# Set OpenMP environment for NUMA
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Test 1: Sequential baseline
echo "--- Sequential Baseline ---"
export OMP_NUM_THREADS=1
./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/sequential.txt 2>&1
sleep 1

# Test 2: OpenMP scaling (1, 2, 4, 8 threads)
echo ""
echo "--- OpenMP Scaling ---"
for threads in 1 2 4 8; do
    echo "  Testing with $threads OpenMP threads..."
    export OMP_NUM_THREADS=$threads
    ./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/openmp_${threads}threads.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
done

# Test 3: Pure MPI (1, 2, 4, 8 processes)
echo ""
echo "--- Pure MPI Scaling ---"
if command -v mpirun &> /dev/null; then
    for procs in 1 2 4 8; do
        echo "  Testing with $procs MPI processes..."
        # NUMA-aware binding
        mpirun -np $procs --bind-to socket --map-by socket \
            ./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/mpi_${procs}procs.txt 2>&1 || echo "  Warning: Test failed"
        sleep 1
    done
else
    echo "  Warning: mpirun not found, skipping MPI tests"
fi

# Test 4: Hybrid configurations (NUMA-optimized)
echo ""
echo "--- Hybrid MPI+OpenMP (NUMA-Optimized) ---"
if command -v mpirun &> /dev/null; then
    # 2 MPI processes (1 per NUMA node) × 4 OpenMP threads
    echo "  Testing 2 MPI processes × 4 OpenMP threads (optimal for 2 NUMA nodes)..."
    export OMP_NUM_THREADS=4
    mpirun -np 2 --bind-to socket --map-by socket \
        ./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/hybrid_2x4.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
    
    # 1 MPI process × 8 OpenMP threads (single node)
    echo "  Testing 1 MPI process × 8 OpenMP threads..."
    export OMP_NUM_THREADS=8
    mpirun -np 1 ./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/hybrid_1x8.txt 2>&1 || echo "  Warning: Test failed"
    sleep 1
    
    # 4 MPI processes × 2 OpenMP threads
    echo "  Testing 4 MPI processes × 2 OpenMP threads..."
    export OMP_NUM_THREADS=2
    mpirun -np 4 --bind-to socket --map-by socket \
        ./bin/performance $MATRIX_SIZE > ${OUTPUT_DIR}/hybrid_4x2.txt 2>&1 || echo "  Warning: Test failed"
else
    echo "  Warning: mpirun not found, skipping Hybrid tests"
fi

echo ""
echo "=========================================="
echo "NUMA-Optimized Benchmark Complete!"
echo "Results saved to: $OUTPUT_DIR"
echo ""
echo "Key configurations tested:"
echo "  - Sequential: Baseline"
echo "  - OpenMP: 1, 2, 4, 8 threads"
echo "  - MPI: 1, 2, 4, 8 processes (NUMA-aware)"
echo "  - Hybrid: 2×4 (optimal for 2 NUMA nodes), 1×8, 4×2"
echo "=========================================="

