# Benchmarking Scripts

Quick reference for running benchmarks on supercomputers.

## 🚀 Quick Start

```bash
# 1. Make scripts executable (already done, but just in case)
chmod +x scripts/*.sh

# 2. Build the project
make clean && make

# 3. Run a benchmark
./scripts/benchmark_strong_scaling.sh 4096
```

## 📋 Available Scripts

### 1. `benchmark_strong_scaling.sh` - Strong Scaling Test
**Purpose**: Measure speedup with fixed problem size, varying parallelism

**Usage**:
```bash
./scripts/benchmark_strong_scaling.sh [matrix_size]
```

**Example**:
```bash
# Test with 4096×4096 matrices
./scripts/benchmark_strong_scaling.sh 4096

# Test with 2048×2048 matrices
./scripts/benchmark_strong_scaling.sh 2048
```

**What it tests**:
- OpenMP: 1, 2, 4, 8, 16, 32, 64 threads
- MPI: 1, 2, 4, 8, 16, 32, 64 processes
- Hybrid: 1, 2, 4, 8 nodes × 8 threads

**Output**: `results/strong_scaling_<size>/`

---

### 2. `benchmark_weak_scaling.sh` - Weak Scaling Test
**Purpose**: Measure scalability when problem size grows with parallelism

**Usage**:
```bash
./scripts/benchmark_weak_scaling.sh [base_size_per_process]
```

**Example**:
```bash
# Each process gets 2048×2048 submatrix
./scripts/benchmark_weak_scaling.sh 2048
```

**What it tests**:
- MPI: 1, 2, 4, 8, 16, 32 processes
  - Size = base_size × num_processes
- Hybrid: 1, 2, 4, 8 nodes × 8 threads
  - Size = base_size × (nodes × threads)

**Output**: `results/weak_scaling/`

---

### 3. `benchmark_all_implementations.sh` - Comprehensive Test
**Purpose**: Test all implementations across multiple matrix sizes

**Usage**:
```bash
./scripts/benchmark_all_implementations.sh [size1] [size2] ...
```

**Example**:
```bash
# Default sizes: 512, 1024, 2048, 4096
./scripts/benchmark_all_implementations.sh

# Custom sizes
./scripts/benchmark_all_implementations.sh 1024 2048 4096 8192
```

**What it tests**:
- Sequential (baseline)
- OpenMP (8 threads)
- MPI (8 processes)
- Hybrid (2 nodes × 4 threads)

**Output**: `results/comprehensive/`

---

### 4. `benchmark_slurm.sh` - SLURM Job Script
**Purpose**: Submit comprehensive benchmark as SLURM job

**Usage**:
```bash
sbatch scripts/benchmark_slurm.sh
```

**Before using**:
1. Edit the script to match your cluster's configuration
2. Adjust `--partition`, `--nodes`, `--time`, etc.
3. Add module load commands if needed

**Check status**:
```bash
squeue -u $USER
```

**View output**:
```bash
cat benchmark_<jobid>.out
```

---

### 5. `benchmark_pbs.sh` - PBS/Torque Job Script
**Purpose**: Submit benchmark as PBS job

**Usage**:
```bash
qsub scripts/benchmark_pbs.sh
```

**Check status**:
```bash
qstat -u $USER
```

---

### 6. `analyze_results.sh` - Results Analysis
**Purpose**: Extract key metrics from benchmark output files

**Usage**:
```bash
./scripts/analyze_results.sh [results_directory]
```

**Example**:
```bash
./scripts/analyze_results.sh results/strong_scaling_4096
```

**Output**: Creates `results_summary.txt` with extracted metrics

---

## 🎯 Common Workflows

### Workflow 1: Quick Single-Node Test
```bash
# Test OpenMP on login node (if allowed)
export OMP_NUM_THREADS=8
./bin/performance 1024 2048
```

### Workflow 2: Strong Scaling Analysis
```bash
# Request interactive node
srun --pty --time=02:00:00 --nodes=1 --ntasks-per-node=8 bash

# Run strong scaling
./scripts/benchmark_strong_scaling.sh 4096
```

### Workflow 3: Full Benchmark Suite
```bash
# Submit as batch job
sbatch scripts/benchmark_slurm.sh

# Or run interactively
./scripts/benchmark_all_implementations.sh
```

### Workflow 4: Custom Matrix Sizes
```bash
# Direct performance test with custom sizes
./bin/performance 512 1024 2048 4096 8192

# Or via script
./scripts/benchmark_all_implementations.sh 512 1024 2048 4096
```

---

## 📊 Understanding Output

### Performance Test Output
```
[4096×4096] Benchmarking...
    Testing Naive...           1914.51 ms      1.04 GFLOPS
    Testing Strassen...        1476.93 ms      1.35 GFLOPS      1.30x
    Testing OpenMP...           381.02 ms      5.25 GFLOPS      5.02x
```

**Metrics**:
- **Time (ms)**: Wall-clock execution time
- **GFLOPS**: Giga Floating-Point Operations Per Second
- **Speedup**: Relative to naive implementation

### Strong Scaling Results
Look for:
- **Speedup**: Should increase with more processors
- **Efficiency**: Speedup / Processors (ideally > 80%)
- **Diminishing returns**: Efficiency decreases with more processors

### Weak Scaling Results
Look for:
- **Time**: Should remain nearly constant (good weak scaling)
- **GFLOPS**: Should increase linearly with problem size

---

## 🔧 Customization

### Adjust Matrix Sizes
Edit the scripts or pass sizes as arguments:
```bash
./scripts/benchmark_all_implementations.sh 1024 2048 4096 8192
```

### Adjust Thread/Process Counts
Edit the scripts:
```bash
# In benchmark_strong_scaling.sh
for threads in 1 2 4 8 16 32 64; do  # Modify this line
```

### Change Output Directory
Edit scripts or set environment variable:
```bash
export RESULTS_DIR="my_results"
# Scripts will use this if you modify them
```

---

## 📝 Tips

1. **Start Small**: Test with small matrices first (512, 1024)
2. **Check Resources**: Ensure you have enough memory for large matrices
3. **Multiple Runs**: Run 3-5 times and average for statistical significance
4. **Monitor System**: Use `htop` to check CPU/memory usage
5. **Save Everything**: Keep all output files for later analysis
6. **Document Environment**: Note compiler versions, module versions

---

## 🐛 Troubleshooting

### Script fails: "bin/performance not found"
```bash
# Build first
make clean && make
```

### MPI tests skipped: "mpirun not found"
```bash
# Load MPI module
module load openmpi
# or
module load intel-mpi
```

### Out of memory
```bash
# Reduce matrix size
./scripts/benchmark_strong_scaling.sh 2048  # Instead of 4096
```

### Job timeout
```bash
# Increase time limit in job script
#SBATCH --time=08:00:00  # 8 hours instead of 4
```

---

## 📚 See Also

- **SUPERCOMPUTER_GUIDE.md**: Complete guide for supercomputer use
- **README.md**: Project documentation
- **REFACTORING_SUMMARY.md**: Performance optimizations

---

**Happy Benchmarking!** 🚀

