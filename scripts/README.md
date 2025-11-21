# Benchmark Scripts

## Main Script: `benchmark.sh`

Single interface for all benchmarks.

### Usage

```bash
./scripts/benchmark.sh [command] [options]
```

### Commands

| Command | Description | Example |
|---------|-------------|---------|
| `quick` | Naive vs OpenMP | `./scripts/benchmark.sh quick 2048` |
| `all` | All implementations | `./scripts/benchmark.sh all 512 1024 2048` |
| `scaling` | Strong scaling | `./scripts/benchmark.sh scaling 4096` |
| `numa` | NUMA-optimized | `./scripts/benchmark.sh numa 4096` |
| `help` | Show help | `./scripts/benchmark.sh help` |

### Examples

```bash
# Quick test (default: 2048)
./scripts/benchmark.sh quick

# Full suite (default: 512 1024 2048 4096)
./scripts/benchmark.sh all

# Custom sizes
./scripts/benchmark.sh all 1024 2048 4096 8192

# Scaling analysis
./scripts/benchmark.sh scaling 4096

# NUMA test
./scripts/benchmark.sh numa 4096
```

## Direct Binary Usage

```bash
export OMP_NUM_THREADS=8
./bin/performance 512 1024 2048 4096
```

## Output

Results displayed directly in terminal.
To save output:
```bash
./scripts/benchmark.sh all > results.txt
```
