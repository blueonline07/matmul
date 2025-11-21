# Benchmarking Scripts

Simple, clean interface for running benchmarks.

## Quick Start

```bash
# Make scripts executable
chmod +x scripts/*.sh

# Build first
make clean && make

# Run benchmarks
./scripts/benchmark.sh quick 2048
```

## Main Script: `benchmark.sh`

Single entry point for all benchmarks.

### Commands

#### `all [sizes...]` - Test all implementations
```bash
./scripts/benchmark.sh all 512 1024 2048 4096
./scripts/benchmark.sh all  # Uses defaults: 512 1024 2048 4096
```
Tests: Naive, Strassen, OpenMP, StrassenOpenMP across multiple sizes

#### `scaling [size]` - Strong scaling test
```bash
./scripts/benchmark.sh scaling 4096
./scripts/benchmark.sh scaling  # Default: 4096
```
Tests: OpenMP (1, 2, 4, 8 threads) and MPI (1, 2, 4, 8 processes)

#### `numa [size]` - NUMA-optimized test
```bash
./scripts/benchmark.sh numa 4096
./scripts/benchmark.sh numa  # Default: 4096
```
Tests: Sequential, OpenMP, Hybrid (for 2-socket systems)

#### `quick [size]` - Quick Naive vs OpenMP
```bash
./scripts/benchmark.sh quick 2048
./scripts/benchmark.sh quick  # Default: 2048
```
Fast comparison of Naive and OpenMP implementations

## Job Scheduler: `benchmark_slurm.sh`

Submit as SLURM job:
```bash
# Edit script first to match your cluster
nano scripts/benchmark_slurm.sh

# Submit
sbatch scripts/benchmark_slurm.sh

# Check status
squeue -u $USER
```

## Examples

```bash
# Quick test
./scripts/benchmark.sh quick

# Full benchmark suite
./scripts/benchmark.sh all 1024 2048 4096

# Scaling analysis
./scripts/benchmark.sh scaling 4096

# NUMA test
./scripts/benchmark.sh numa 4096
```

## Direct Usage

You can also use the binary directly:
```bash
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096
```
