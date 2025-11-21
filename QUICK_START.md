# Quick Start Guide

## 1. Build
```bash
make clean && make
```

## 2. Run Tests
```bash
./bin/correctness  # Verify correctness
```

## 3. Benchmark
```bash
# Quick comparison
./scripts/benchmark.sh quick 2048

# Full suite
./scripts/benchmark.sh all 512 1024 2048 4096
```

## Commands Reference

```bash
# Show help
./scripts/benchmark.sh help

# Different modes
./scripts/benchmark.sh quick 2048        # Naive vs OpenMP
./scripts/benchmark.sh all 1024 2048     # All algorithms
./scripts/benchmark.sh scaling 4096      # Strong scaling
./scripts/benchmark.sh numa 4096         # NUMA-aware

# Direct binary
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096

# With MPI
mpirun -np 4 ./bin/performance 4096
```

## Your System Setup

**CPU**: 8 cores (2 sockets × 4 cores), 2 NUMA nodes  
**Memory**: 15 GB RAM

**Optimal configuration**:
```bash
export OMP_NUM_THREADS=4
mpirun -np 2 --bind-to socket ./bin/performance 4096
```

**Safe matrix sizes**: 512, 1024, 2048, 4096  
**Maximum**: ~8192×8192

## Troubleshooting

```bash
# Build fails
make clean && make

# Out of memory
./bin/performance 2048  # Use smaller sizes

# Permission denied
chmod +x scripts/*.sh
```

## More Info

- **README.md** - Full documentation
- **CONFIGURATION_ANALYSIS.md** - System specs
- **scripts/README.md** - Script details

