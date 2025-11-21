# Matrix Multiplication Benchmarks

High-performance matrix multiplication using various algorithms and parallelization techniques.

## Quick Start

```bash
# Build
make clean && make

# Run tests
./bin/correctness

# Benchmark
./scripts/benchmark.sh quick 2048
```

## Implementations

| Algorithm | Status | Description |
|-----------|--------|-------------|
| **Naive** | ✅ | Cache-optimized (ikj ordering) |
| **Strassen** | ✅ | Divide-and-conquer O(n^2.807) |
| **OpenMP** | ✅ | Shared-memory parallelism |
| **StrassenOpenMP** | ✅ | Parallel Strassen with OpenMP tasks |
| **MPI** | ⚠️ | Placeholder |
| **Hybrid** | ⚠️ | Placeholder |

## Benchmarking

### Quick tests
```bash
# Naive vs OpenMP
./scripts/benchmark.sh quick 2048

# All implementations
./scripts/benchmark.sh all 512 1024 2048 4096

# Strong scaling
./scripts/benchmark.sh scaling 4096

# NUMA-optimized
./scripts/benchmark.sh numa 4096
```

### Direct method
```bash
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096
```

## For Your System

Your system: **8 cores (2 NUMA nodes), 15GB RAM**

Recommended configuration:
```bash
# Best: Hybrid 2 MPI × 4 OpenMP
export OMP_NUM_THREADS=4
mpirun -np 2 --bind-to socket ./bin/performance 4096

# Or: Full OpenMP
export OMP_NUM_THREADS=8
./bin/performance 4096
```

Safe matrix sizes: **512, 1024, 2048, 4096**  
Maximum safe: **~8192×8192**

## Files

- `scripts/benchmark.sh` - Main benchmark script
- `CONFIGURATION_ANALYSIS.md` - System specs and tuning
- `scripts/README.md` - Script documentation

## Performance Expectations (4096×4096)

| Implementation | Time | GFLOPS | Speedup |
|----------------|------|--------|---------|
| Naive | ~2000 ms | ~1.0 | 1.0× |
| Strassen | ~1500 ms | ~1.4 | 1.3× |
| OpenMP | ~400 ms | ~5.0 | 5.0× |
| StrassenOpenMP | ~350 ms | ~5.7 | 5.7× |
