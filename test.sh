#!/bin/bash

# Default values
N=1000

# Parse arguments
for arg in "$@"; do
    if [[ $arg == N=* ]]; then
        N="${arg#N=}"
    fi
done

# Helper function for division
divide() {
    awk "BEGIN {printf \"%.6f\", $1 / $2}"
}

# Output file for results (CSV format)
OUTPUT_FILE="results_N${N}.csv"

echo "Running tests with N=$N"
echo "Results will be saved to: $OUTPUT_FILE"
echo "================================"
echo ""

# Initialize CSV file with header
echo "method,threads,procs,hosts,time,speedup,efficiency" > "$OUTPUT_FILE"

# Build all tests first
echo "Building all tests..."
make all > /dev/null 2>&1
echo ""

# Run test_serial (output: naive, strassen)
echo "1. Running test_serial (N=$N)..."
SERIAL_OUTPUT=$(./bin/test_serial $N 2>&1)
echo "$SERIAL_OUTPUT"
SERIAL_LINES=($(echo "$SERIAL_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$'))
SERIAL_NAIVE_TIME=${SERIAL_LINES[0]}
SERIAL_STRASSEN_TIME=${SERIAL_LINES[1]}

# Baseline for speedup calculation (serial naive)
BASELINE_TIME=$SERIAL_NAIVE_TIME

# Write serial results to CSV
echo "serial_naive,1,1,\"\",$SERIAL_NAIVE_TIME,1.0,1.0" >> "$OUTPUT_FILE"
SPEEDUP=$(divide $BASELINE_TIME $SERIAL_STRASSEN_TIME)
echo "serial_strassen,1,1,\"\",$SERIAL_STRASSEN_TIME,$SPEEDUP,1.0" >> "$OUTPUT_FILE"
echo ""

# Run test_omp with 8 threads (output: naive, strassen)
echo "2. Running test_omp (N=$N, threads=8)..."
OMP8_OUTPUT=$(OMP_NUM_THREADS=8 ./bin/test_omp $N 2>&1)
echo "$OMP8_OUTPUT"
OMP8_LINES=($(echo "$OMP8_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$'))
OMP8_NAIVE_TIME=${OMP8_LINES[0]}
OMP8_STRASSEN_TIME=${OMP8_LINES[1]}

# Calculate speedup and efficiency for OMP 8 threads
SPEEDUP_NAIVE=$(divide $BASELINE_TIME $OMP8_NAIVE_TIME)
EFFICIENCY_NAIVE=$(divide $SPEEDUP_NAIVE 8)
echo "omp_naive,8,1,\"\",$OMP8_NAIVE_TIME,$SPEEDUP_NAIVE,$EFFICIENCY_NAIVE" >> "$OUTPUT_FILE"

SPEEDUP_STRASSEN=$(divide $BASELINE_TIME $OMP8_STRASSEN_TIME)
EFFICIENCY_STRASSEN=$(divide $SPEEDUP_STRASSEN 8)
echo "omp_strassen,8,1,\"\",$OMP8_STRASSEN_TIME,$SPEEDUP_STRASSEN,$EFFICIENCY_STRASSEN" >> "$OUTPUT_FILE"
echo ""

# Run test_omp with 4 threads (output: naive, strassen)
echo "3. Running test_omp (N=$N, threads=4)..."
OMP4_OUTPUT=$(OMP_NUM_THREADS=4 ./bin/test_omp $N 2>&1)
echo "$OMP4_OUTPUT"
OMP4_LINES=($(echo "$OMP4_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$'))
OMP4_NAIVE_TIME=${OMP4_LINES[0]}
OMP4_STRASSEN_TIME=${OMP4_LINES[1]}

# Calculate speedup and efficiency for OMP 4 threads
SPEEDUP_NAIVE=$(divide $BASELINE_TIME $OMP4_NAIVE_TIME)
EFFICIENCY_NAIVE=$(divide $SPEEDUP_NAIVE 4)
echo "omp_naive,4,1,\"\",$OMP4_NAIVE_TIME,$SPEEDUP_NAIVE,$EFFICIENCY_NAIVE" >> "$OUTPUT_FILE"

SPEEDUP_STRASSEN=$(divide $BASELINE_TIME $OMP4_STRASSEN_TIME)
EFFICIENCY_STRASSEN=$(divide $SPEEDUP_STRASSEN 4)
echo "omp_strassen,4,1,\"\",$OMP4_STRASSEN_TIME,$SPEEDUP_STRASSEN,$EFFICIENCY_STRASSEN" >> "$OUTPUT_FILE"
echo ""

# Define host configurations: 0, 1, 2, 3 hosts
HOST_NODES=("MPI-node14" "MPI-node15" "MPI-node16")

# Loop through host configurations (0 to 3 hosts)
for NUM_HOSTS in 0 1 2 3; do
    if [ $NUM_HOSTS -eq 0 ]; then
        HOSTS=""
        PROCS=8  # Default local processes
    else
        # Build hosts string from first NUM_HOSTS nodes
        HOSTS=""
        for ((i=0; i<NUM_HOSTS; i++)); do
            if [ $i -gt 0 ]; then
                HOSTS="${HOSTS},"
            fi
            HOSTS="${HOSTS}${HOST_NODES[$i]}"
        done
        PROCS=$((NUM_HOSTS * 8))
    fi
    
    echo "================================"
    if [ $NUM_HOSTS -eq 0 ]; then
        echo "Testing with 0 hosts (local)"
    else
        echo "Testing with $NUM_HOSTS host(s): $HOSTS"
    fi
    echo "Processes: $PROCS"
    echo "================================"
    echo ""
    
    # Run test_mpi (output: naive only)
    echo "Running test_mpi (N=$N, procs=$PROCS, hosts=$HOSTS)..."
    if [ -z "$HOSTS" ]; then
        MPI_OUTPUT=$(mpirun -np $PROCS ./bin/test_mpi $N 2>&1)
    else
        MPI_OUTPUT=$(mpirun -np $PROCS -hosts $HOSTS ./bin/test_mpi $N 2>&1)
    fi
    echo "$MPI_OUTPUT"
    MPI_LINE=$(echo "$MPI_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$' | head -n 1)
    
    if [ -n "$MPI_LINE" ]; then
        # Calculate speedup and efficiency for MPI (efficiency = speedup / procs)
        SPEEDUP=$(divide $BASELINE_TIME $MPI_LINE)
        EFFICIENCY=$(divide $SPEEDUP $PROCS)
        echo "mpi_naive,0,$PROCS,\"$HOSTS\",$MPI_LINE,$SPEEDUP,$EFFICIENCY" >> "$OUTPUT_FILE"
    fi
    echo ""
    
    # Run test_hybrid (output: naive only)
    echo "Running test_hybrid (N=$N, procs=$PROCS, threads=8, hosts=$HOSTS)..."
    if [ -z "$HOSTS" ]; then
        HYBRID_OUTPUT=$(OMP_NUM_THREADS=8 mpirun -np $PROCS ./bin/test_hybrid $N 2>&1)
    else
        HYBRID_OUTPUT=$(OMP_NUM_THREADS=8 mpirun -np $PROCS -hosts $HOSTS ./bin/test_hybrid $N 2>&1)
    fi
    echo "$HYBRID_OUTPUT"
    HYBRID_LINE=$(echo "$HYBRID_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$' | head -n 1)
    
    if [ -n "$HYBRID_LINE" ]; then
        # Calculate speedup and efficiency for hybrid (efficiency = speedup / threads)
        SPEEDUP=$(divide $BASELINE_TIME $HYBRID_LINE)
        EFFICIENCY=$(divide $SPEEDUP 8)
        echo "hybrid_naive,8,$PROCS,\"$HOSTS\",$HYBRID_LINE,$SPEEDUP,$EFFICIENCY" >> "$OUTPUT_FILE"
    fi
    echo ""
    
    # Run test_strassen (output: mpi strassen, hybrid strassen) - needs exactly 7 processes
    if [ $NUM_HOSTS -eq 0 ] || [ $NUM_HOSTS -ge 1 ]; then
        echo "Running test_strassen (N=$N, procs=7, hosts=$HOSTS)..."
        if [ -z "$HOSTS" ]; then
            STRASSEN_OUTPUT=$(mpirun -np 7 ./bin/test_strassen $N 2>&1)
        else
            STRASSEN_OUTPUT=$(mpirun -np 7 -hosts $HOSTS ./bin/test_strassen $N 2>&1)
        fi
        echo "$STRASSEN_OUTPUT"
        STRASSEN_LINES=($(echo "$STRASSEN_OUTPUT" | grep -E '^[0-9]+\.[0-9]+$'))
        MPI_STRASSEN_TIME=${STRASSEN_LINES[0]}
        HYBRID_STRASSEN_TIME=${STRASSEN_LINES[1]}
        
        if [ -n "$MPI_STRASSEN_TIME" ]; then
            # Calculate speedup and efficiency for MPI strassen (efficiency = speedup / procs)
            SPEEDUP=$(divide $BASELINE_TIME $MPI_STRASSEN_TIME)
            EFFICIENCY=$(divide $SPEEDUP 7)
            echo "mpi_strassen,0,7,\"$HOSTS\",$MPI_STRASSEN_TIME,$SPEEDUP,$EFFICIENCY" >> "$OUTPUT_FILE"
        fi
        
        if [ -n "$HYBRID_STRASSEN_TIME" ]; then
            # Calculate speedup and efficiency for hybrid strassen (efficiency = speedup / threads)
            SPEEDUP=$(divide $BASELINE_TIME $HYBRID_STRASSEN_TIME)
            EFFICIENCY=$(divide $SPEEDUP 8)
            echo "hybrid_strassen,8,7,\"$HOSTS\",$HYBRID_STRASSEN_TIME,$SPEEDUP,$EFFICIENCY" >> "$OUTPUT_FILE"
        fi
        echo ""
    fi
done

echo "================================"
echo "Tests completed! Results saved to: $OUTPUT_FILE"
