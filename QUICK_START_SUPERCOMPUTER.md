# Quick Start: Supercomputer Benchmarking

## 🚀 Setup (5 Steps)

```bash
# 1. Transfer code
cd ~/workspace && git clone <repo> matmul && cd matmul

# 2. Load modules
module load gcc openmpi

# 3. Build
make clean && make

# 4. Set OpenMP (8 threads for your system)
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# 5. Run benchmark
./bin/performance 512 1024 2048 4096
```

## 📊 Benchmarking Naive vs OpenMP

### Method 1: Direct (Simplest)
```bash
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Run and look for Naive/OpenMP rows
./bin/performance 512 1024 2048 4096
```

### Method 2: Using Scripts
```bash
# Strong scaling (varying thread counts)
./scripts/benchmark_strong_scaling.sh 4096

# NUMA-optimized (for 2-socket systems)
./scripts/benchmark_numa_optimized.sh 4096

# All implementations
./scripts/benchmark_all_implementations.sh
```

## 📋 Essential Commands

```bash
# Custom matrix sizes
./bin/performance 512 1024 2048 4096

# Different thread counts
export OMP_NUM_THREADS=1 && ./bin/performance 4096
export OMP_NUM_THREADS=4 && ./bin/performance 4096
export OMP_NUM_THREADS=8 && ./bin/performance 4096

# With MPI
mpirun -np 4 ./bin/performance 4096

# Hybrid (2 nodes × 4 threads)
export OMP_NUM_THREADS=4
mpirun -np 2 --bind-to socket ./bin/performance 4096
```

## 📁 Results

All results saved in `results/` directory:
```
results/
├── strong_scaling_4096/
├── weak_scaling/
└── comprehensive/
```

## 🐛 Troubleshooting

```bash
# "mpirun not found"
module load openmpi

# "Out of memory"
./bin/performance 2048  # Reduce size

# "Permission denied"
chmod +x scripts/*.sh
```

## 📚 More Info

- **CONFIGURATION_ANALYSIS.md** - Your system specs and recommendations
- **scripts/README.md** - Detailed script documentation
