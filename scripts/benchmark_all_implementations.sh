#!/bin/bash
# Comprehensive benchmark of all implementations
# Tests all algorithms across multiple matrix sizes

set -e  # Exit on error

OUTPUT_DIR="results/comprehensive"
mkdir -p $OUTPUT_DIR

# Matrix sizes to test (adjust based on available memory)
SIZES=(512 1024 2048 4096)
if [ -n "$1" ]; then
    # Custom sizes provided
    SIZES=("$@")
fi

echo "=========================================="
echo "Comprehensive Benchmark Suite"
echo "Matrix Sizes: ${SIZES[@]}"
echo "Output Directory: $OUTPUT_DIR"
echo "=========================================="
echo ""

# Ensure binary exists
if [ ! -f "./bin/performance" ]; then
    echo "Error: ./bin/performance not found. Run 'make' first."
    exit 1
fi

# Set OpenMP environment
export OMP_PLACES=cores
export OMP_PROC_BIND=close

for size in "${SIZES[@]}"; do
    echo "--- Testing ${size}×${size} matrices ---"
    
    # Sequential implementations (baseline)
    echo "  Sequential tests..."
    export OMP_NUM_THREADS=1
    ./bin/performance > ${OUTPUT_DIR}/seq_${size}.txt 2>&1 || echo "    Warning: Sequential test failed"
    sleep 1
    
    # OpenMP (8 threads - adjust based on your system)
    echo "  OpenMP (8 threads)..."
    export OMP_NUM_THREADS=8
    ./bin/performance > ${OUTPUT_DIR}/openmp8_${size}.txt 2>&1 || echo "    Warning: OpenMP test failed"
    sleep 1
    
    # Check if mpirun is available
    if command -v mpirun &> /dev/null; then
        # MPI (8 processes)
        echo "  MPI (8 processes)..."
        mpirun -np 8 ./bin/performance > ${OUTPUT_DIR}/mpi8_${size}.txt 2>&1 || echo "    Warning: MPI test failed"
        sleep 1
        
        # Hybrid (2 nodes × 4 threads = 8 total)
        echo "  Hybrid (2 nodes × 4 threads)..."
        export OMP_NUM_THREADS=4
        mpirun -np 2 ./bin/performance > ${OUTPUT_DIR}/hybrid_2x4_${size}.txt 2>&1 || echo "    Warning: Hybrid test failed"
        sleep 1
    else
        echo "  Skipping MPI/Hybrid tests (mpirun not found)"
    fi
    
    echo ""
done

echo "=========================================="
echo "Comprehensive Benchmark Complete!"
echo "Results saved to: $OUTPUT_DIR"
echo "=========================================="

