#!/bin/bash
# Main Benchmark Script - Clean Interface
# Usage: ./scripts/benchmark.sh [option] [args...]

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check if binary exists
check_binary() {
    if [ ! -f "./bin/performance" ]; then
        echo -e "${RED}Error: ./bin/performance not found. Run 'make' first.${NC}"
        exit 1
    fi
}

# Print usage
usage() {
    echo "Matrix Multiplication Benchmark Suite"
    echo ""
    echo "Usage: ./scripts/benchmark.sh [command] [options]"
    echo ""
    echo "Commands:"
    echo "  all [sizes...]           - Test all implementations (default: 512 1024 2048 4096)"
    echo "  scaling [size]           - Strong scaling test (default: 4096)"
    echo "  numa [size]              - NUMA-optimized test (default: 4096)"
    echo "  quick [size]              - Quick test: Naive vs OpenMP (default: 2048)"
    echo "  help                     - Show this help"
    echo ""
    echo "Examples:"
    echo "  ./scripts/benchmark.sh all 1024 2048 4096"
    echo "  ./scripts/benchmark.sh scaling 4096"
    echo "  ./scripts/benchmark.sh quick 2048"
    echo ""
}

# Quick test: Naive vs OpenMP
quick_test() {
    local size=${1:-2048}
    check_binary
    
    echo -e "${GREEN}=== Quick Test: Naive vs OpenMP ===${NC}"
    echo "Matrix Size: ${size}×${size}"
    echo ""
    
    export OMP_NUM_THREADS=8
    
    ./bin/performance $size | grep -E "(Testing|Naive|OpenMP|Size|GFLOPS|Speedup)"
}

# All implementations
run_all() {
    local sizes=("${@:-512 1024 2048 4096}")
    check_binary
    
    echo -e "${GREEN}=== Comprehensive Benchmark ===${NC}"
    echo "Sizes: ${sizes[@]}"
    echo ""
    
    for size in "${sizes[@]}"; do
        echo "--- ${size}×${size} ---"
        export OMP_NUM_THREADS=8
        ./bin/performance $size
        echo ""
    done
}

# Strong scaling
run_scaling() {
    local size=${1:-4096}
    check_binary
    
    echo -e "${GREEN}=== Strong Scaling Test ===${NC}"
    echo "Matrix Size: ${size}×${size}"
    echo ""
    
    echo "OpenMP Scaling:"
    for threads in 1 2 4 8; do
        if [ $threads -gt $(nproc 2>/dev/null || echo 8) ]; then
            continue
        fi
        echo "  Threads: $threads"
        export OMP_NUM_THREADS=$threads
        ./bin/performance $size | grep -E "(Naive|OpenMP)" | head -2
    done
    
    if command -v mpirun &> /dev/null; then
        echo ""
        echo "MPI Scaling:"
        for procs in 1 2 4 8; do
            echo "  Processes: $procs"
            mpirun -np $procs ./bin/performance $size 2>&1 | grep -E "(Naive|MPI)" | head -2
        done
    fi
}

# NUMA optimized
run_numa() {
    local size=${1:-4096}
    check_binary
    
    echo -e "${GREEN}=== NUMA-Optimized Test ===${NC}"
    echo "Matrix Size: ${size}×${size}"
    echo ""
    
    # Sequential
    echo "Sequential:"
    export OMP_NUM_THREADS=1
    ./bin/performance $size | grep -E "(Naive)" | head -1
    
    # OpenMP
    echo "OpenMP (8 threads):"
    export OMP_NUM_THREADS=8
    ./bin/performance $size | grep -E "(OpenMP)" | head -1
    
    # Hybrid if MPI available
    if command -v mpirun &> /dev/null; then
        echo "Hybrid (2×4):"
        export OMP_NUM_THREADS=4
        mpirun -np 2 --bind-to socket ./bin/performance $size 2>&1 | grep -E "(Hybrid|MPI)" | head -1
    fi
}

# Main
case "${1:-help}" in
    all)
        shift
        run_all "$@"
        ;;
    scaling)
        run_scaling "${2:-4096}"
        ;;
    numa)
        run_numa "${2:-4096}"
        ;;
    quick)
        quick_test "${2:-2048}"
        ;;
    help|--help|-h)
        usage
        ;;
    *)
        echo -e "${RED}Unknown command: $1${NC}"
        echo ""
        usage
        exit 1
        ;;
esac

