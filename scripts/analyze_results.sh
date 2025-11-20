#!/bin/bash
# Simple script to extract key metrics from benchmark results
# Creates summary tables from output files

RESULTS_DIR=${1:-results}
OUTPUT_FILE="results_summary.txt"

echo "=========================================="
echo "Benchmark Results Analysis"
echo "Analyzing: $RESULTS_DIR"
echo "=========================================="
echo ""

# Function to extract metrics from a result file
extract_metrics() {
    local file=$1
    if [ ! -f "$file" ]; then
        return
    fi
    
    # Extract GFLOPS, time, speedup from performance output
    gflops=$(grep -i "gflops" "$file" | tail -1 | grep -oE "[0-9]+\.[0-9]+" | head -1 || echo "N/A")
    time_ms=$(grep -i "time.*ms" "$file" | tail -1 | grep -oE "[0-9]+\.[0-9]+" | head -1 || echo "N/A")
    speedup=$(grep -i "speedup" "$file" | tail -1 | grep -oE "[0-9]+\.[0-9]+x" | head -1 || echo "N/A")
    
    echo "$file: GFLOPS=$gflops, Time=$time_ms ms, Speedup=$speedup"
}

# Analyze all result files
echo "--- Performance Summary ---" > $OUTPUT_FILE
echo "" >> $OUTPUT_FILE

# Find all result files
find $RESULTS_DIR -name "*.txt" -type f | while read file; do
    extract_metrics "$file" >> $OUTPUT_FILE
done

cat $OUTPUT_FILE
echo ""
echo "Full summary saved to: $OUTPUT_FILE"

