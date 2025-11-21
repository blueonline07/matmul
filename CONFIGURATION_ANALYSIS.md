# System Configuration Analysis

## Your System Specs

- **CPU**: Intel Xeon E5-2680 v3 @ 2.50GHz
- **Cores**: 8 physical (2 sockets × 4 cores)
- **NUMA**: 2 nodes (CPUs 0-3 on node0, CPUs 4-7 on node1)
- **Memory**: 15 GB RAM
- **MPI**: OpenMPI 4.1.2

## Recommended Matrix Sizes

```bash
# Safe sizes (recommended)
512 1024 2048 4096

# Maximum safe: ~8192×8192
# (Each matrix ~1.5GB, need 3 matrices = ~4.5GB total)
```

## Optimal Configuration

### Best for Your System: Hybrid (2×4)
```bash
# 2 MPI processes (1 per NUMA node) × 4 OpenMP threads
export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=close

mpirun -np 2 --bind-to socket --map-by socket ./bin/performance 4096
```

### OpenMP Only (Single Node)
```bash
# Use all 8 cores
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close
./bin/performance 4096
```

## Expected Performance (4096×4096)

| Configuration | Time (s) | GFLOPS | Speedup |
|--------------|----------|--------|---------|
| Sequential   | ~120     | ~1.1   | 1.00×   |
| OpenMP (8)   | ~18      | ~7.5   | 6.67×   |
| Hybrid (2×4) | ~17      | ~8.0   | 7.06×   |

## Quick Test Commands

```bash
# 1. Sequential baseline
export OMP_NUM_THREADS=1
./bin/performance 4096

# 2. OpenMP (4 threads)
export OMP_NUM_THREADS=4
./bin/performance 4096

# 3. OpenMP (8 threads - all cores)
export OMP_NUM_THREADS=8
./bin/performance 4096

# 4. Hybrid (NUMA-optimized)
export OMP_NUM_THREADS=4
mpirun -np 2 --bind-to socket ./bin/performance 4096
```

## Important Notes

1. **NUMA topology**: Use `--bind-to socket` for MPI to respect NUMA boundaries
2. **Memory limit**: 15 GB total - don't exceed 8192×8192
3. **No hyperthreading**: Max 8-way parallelism
4. **Virtualization**: KVM may add 5-10% overhead

## Recommended Test Sequence

```bash
# 1. Quick validation
export OMP_NUM_THREADS=8
./bin/performance 1024

# 2. NUMA-optimized test
./scripts/benchmark_numa_optimized.sh 4096

# 3. Strong scaling
./scripts/benchmark_strong_scaling.sh 4096
```
