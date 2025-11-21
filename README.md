# Matrix Multiplication Project

High-performance matrix multiplication implementations using various algorithms and parallelization techniques.

## Quick Start

```bash
# Build
make clean && make

# Run correctness tests
./bin/correctness

# Run performance benchmarks
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096
```

## Implemented Algorithms

1. **Naive** ✅ - Optimized ikj loop ordering (cache-efficient)
2. **Strassen** ✅ - Divide-and-conquer O(n^2.807)
3. **OpenMP** ✅ - Shared-memory parallelism
4. **MPI** ⚠️ - Placeholder (see `src/multiply_mpi.cpp`)
5. **Hybrid MPI+OpenMP** ⚠️ - Placeholder

## Building

```bash
make                    # Build all
make performance        # Build performance test only
make clean              # Clean build artifacts
```

## Running Tests

```bash
# Correctness tests
./bin/correctness

# Performance benchmarks
./bin/performance [size1] [size2] ...  # Custom sizes
./bin/performance                      # Default sizes

# With MPI
mpirun -np 4 ./bin/performance 4096
```

## Benchmarking on Supercomputer

See **QUICK_START_SUPERCOMPUTER.md** for detailed instructions.

Quick commands:
```bash
# Load modules
module load gcc openmpi

# Build
make clean && make

# Run benchmarks
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096

# Or use scripts
chmod +x scripts/*.sh
./scripts/benchmark_strong_scaling.sh 4096
```

## Project Structure

```
matmul/
├── include/          # Header files
├── src/              # Implementation files
├── tests/            # Test suites
├── scripts/          # Benchmark scripts
└── bin/              # Compiled executables (created by make)
```

## Documentation

- **QUICK_START_SUPERCOMPUTER.md** - How to run on supercomputers
- **CONFIGURATION_ANALYSIS.md** - System-specific analysis
- **scripts/README.md** - Benchmark script documentation
