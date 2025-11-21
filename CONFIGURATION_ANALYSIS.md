# System Configuration

## Your System

- **CPU**: Intel Xeon E5-2680 v3 @ 2.50GHz
- **Cores**: 8 physical (2 sockets × 4 cores, no hyperthreading)
- **NUMA**: 2 nodes (CPUs 0-3, CPUs 4-7)
- **Memory**: 15 GB RAM
- **MPI**: OpenMPI 4.1.2

## Recommended Sizes

```bash
# Safe sizes
512 1024 2048 4096

# Maximum safe: ~8192×8192
# (Each matrix ~1.5GB × 3 = ~4.5GB total)
```

## Optimal Configuration

### Hybrid (Best)
```bash
export OMP_NUM_THREADS=4
mpirun -np 2 --bind-to socket ./bin/performance 4096
```
2 MPI processes (1 per NUMA node) × 4 OpenMP threads = 8 total

### OpenMP Only
```bash
export OMP_NUM_THREADS=8
./bin/performance 4096
```

## Expected Performance (4096×4096)

| Config | Time | GFLOPS | Speedup |
|--------|------|--------|---------|
| Sequential | ~120s | ~1.1 | 1.0× |
| OpenMP (8) | ~18s | ~7.5 | 6.7× |
| Hybrid (2×4) | ~17s | ~8.0 | 7.1× |

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

## Notes

- **NUMA**: Use `--bind-to socket` for MPI
- **Memory**: Don't exceed 8192×8192 without checking
- **Threads**: Max 8-way parallelism (no hyperthreading)
- **Virtualization**: KVM may add ~5-10% overhead
