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

### Using Scripts (Recommended)
```bash
# Quick test: Naive vs OpenMP
./scripts/benchmark.sh quick 2048

# All implementations
./scripts/benchmark.sh all 512 1024 2048 4096

# Strong scaling
./scripts/benchmark.sh scaling 4096

# NUMA-optimized
./scripts/benchmark.sh numa 4096
```

### Direct Method
```bash
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Run and look for Naive/OpenMP rows
./bin/performance 512 1024 2048 4096
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

Results are displayed directly in the terminal. For saved output, redirect:
```bash
./scripts/benchmark.sh all 1024 2048 > results.txt
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
