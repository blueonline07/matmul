# Benchmarking Scripts

## Quick Start

```bash
chmod +x scripts/*.sh
make clean && make
./scripts/benchmark_strong_scaling.sh 4096
```

## Available Scripts

### `benchmark_strong_scaling.sh`
Strong scaling: fixed size, varying parallelism
```bash
./scripts/benchmark_strong_scaling.sh 4096
```
Tests: OpenMP (1-64 threads), MPI (1-64 processes), Hybrid

### `benchmark_weak_scaling.sh`
Weak scaling: size grows with parallelism
```bash
./scripts/benchmark_weak_scaling.sh 2048
```
Tests: MPI and Hybrid with growing problem sizes

### `benchmark_all_implementations.sh`
Comprehensive test of all algorithms
```bash
./scripts/benchmark_all_implementations.sh 1024 2048 4096
```

### `benchmark_numa_optimized.sh`
NUMA-aware benchmarks (for 2-socket systems)
```bash
./scripts/benchmark_numa_optimized.sh 4096
```

### `benchmark_slurm.sh`
Submit as SLURM job
```bash
sbatch scripts/benchmark_slurm.sh
```

### `benchmark_pbs.sh`
Submit as PBS job
```bash
qsub scripts/benchmark_pbs.sh
```

### `analyze_results.sh`
Extract metrics from results
```bash
./scripts/analyze_results.sh results/
```

## Output

All results saved in `results/` directory:
- `results/strong_scaling_<size>/`
- `results/weak_scaling/`
- `results/comprehensive/`
