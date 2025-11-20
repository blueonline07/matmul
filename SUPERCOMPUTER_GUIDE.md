# Supercomputer Benchmarking Guide

Complete guide for running matrix multiplication benchmarks on supercomputers.

---

## 📋 Table of Contents

1. [Prerequisites](#prerequisites)
2. [Transferring Code to Supercomputer](#transferring-code)
3. [Building on Supercomputer](#building)
4. [Running Benchmarks](#running-benchmarks)
5. [Job Scheduler Examples](#job-schedulers)
6. [Understanding Results](#understanding-results)
7. [Troubleshooting](#troubleshooting)

---

## 🔧 Prerequisites

### Required Software
- **C++ Compiler**: GCC 9+ or Intel C++ with C++17 support
- **MPI**: OpenMPI 4.0+ or Intel MPI
- **OpenMP**: Usually included with compiler
- **Make**: Build system

### Check Available Resources
```bash
# Check CPU info
lscpu
cat /proc/cpuinfo

# Check memory
free -h

# Check MPI
mpirun --version
which mpicc

# Check compiler
g++ --version
```

---

## 📤 Transferring Code to Supercomputer

### Option 1: Git (Recommended)
```bash
# On supercomputer
cd ~/workspace
git clone <your-repo-url> matmul
cd matmul
```

### Option 2: SCP/SFTP
```bash
# From your local machine
scp -r matmul/ username@supercomputer:/home/username/
# or
rsync -avz matmul/ username@supercomputer:/home/username/matmul/
```

### Option 3: Archive and Transfer
```bash
# On local machine
tar -czf matmul.tar.gz matmul/
scp matmul.tar.gz username@supercomputer:/home/username/

# On supercomputer
tar -xzf matmul.tar.gz
cd matmul
```

---

## 🏗️ Building on Supercomputer

### Step 1: Load Required Modules

Most supercomputers use environment modules. Load the necessary ones:

```bash
# Common module commands
module avail              # List available modules
module list               # Show currently loaded modules
module load gcc/11.2.0    # Load GCC
module load openmpi/4.1.0 # Load OpenMPI
module load cmake/3.20.0  # If needed

# Or use system defaults (if modules not available)
# Just proceed to build
```

### Step 2: Update Makefile (if needed)

You may need to adjust the Makefile for the supercomputer:

```makefile
# Check what compilers are available
which g++
which mpic++

# Update Makefile if paths differ
CXX = g++        # or g++-11, g++-9, etc.
MPICXX = mpic++  # or mpicxx
```

### Step 3: Build

```bash
cd matmul
make clean
make

# Verify build
ls -lh bin/
./bin/correctness  # Quick test
```

### Step 4: Test on Login Node (Small Test)

```bash
# Quick test with small matrix
export OMP_NUM_THREADS=4
./bin/performance

# Test MPI (if allowed on login node)
mpirun -np 2 ./bin/performance
```

**Note**: Many supercomputers restrict MPI jobs to compute nodes only.

---

## 🚀 Running Benchmarks

### Quick Start (Interactive)

```bash
# Make scripts executable
chmod +x scripts/*.sh

# Run single benchmark
./scripts/benchmark_strong_scaling.sh 4096

# Run comprehensive suite
./scripts/benchmark_all_implementations.sh
```

### Using Job Schedulers

Most supercomputers require using a job scheduler. See [Job Scheduler Examples](#job-schedulers) below.

---

## 📊 Job Scheduler Examples

### SLURM (Most Common)

#### 1. Interactive Session
```bash
# Request interactive node
srun --pty --time=02:00:00 --nodes=1 --ntasks-per-node=8 bash

# Then run benchmarks
export OMP_NUM_THREADS=8
./bin/performance
```

#### 2. Batch Job
```bash
# Submit job
sbatch scripts/benchmark_slurm.sh

# Check job status
squeue -u $USER

# View output
cat benchmark_<jobid>.out
```

#### 3. Custom SLURM Script
```bash
#!/bin/bash
#SBATCH --job-name=matmul_test
#SBATCH --output=test_%j.out
#SBATCH --time=01:00:00
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=8
#SBATCH --mem=32G

module load gcc openmpi
cd $SLURM_SUBMIT_DIR
make clean && make

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
mpirun -np $SLURM_NTASKS ./bin/performance
```

### PBS/Torque

```bash
# Submit job
qsub scripts/benchmark_pbs.sh

# Check status
qstat -u $USER

# View output
cat benchmark_<jobid>.out
```

### LSF (IBM)

```bash
#!/bin/bash
#BSUB -J matmul_benchmark
#BSUB -o benchmark_%J.out
#BSUB -e benchmark_%J.err
#BSUB -W 04:00
#BSUB -n 32
#BSUB -R "span[ptile=8]"

module load gcc openmpi
cd $LS_SUBCWD
make clean && make

export OMP_NUM_THREADS=8
mpirun -np 4 ./bin/performance
```

---

## 📈 Benchmark Scenarios

### Scenario 1: Strong Scaling

**Goal**: Measure speedup with fixed problem size

```bash
# Fixed 4096×4096 matrix, varying parallelism
./scripts/benchmark_strong_scaling.sh 4096
```

**What to measure**:
- Speedup = Time(1 proc) / Time(N procs)
- Efficiency = Speedup / N
- Ideal: Linear speedup (efficiency = 100%)

### Scenario 2: Weak Scaling

**Goal**: Measure scalability with growing problem size

```bash
# Problem size grows with processors
./scripts/benchmark_weak_scaling.sh 2048
```

**What to measure**:
- Time should remain constant (good weak scaling)
- GFLOPS should increase linearly

### Scenario 3: Comprehensive Comparison

**Goal**: Compare all implementations

```bash
# Test all algorithms across multiple sizes
./scripts/benchmark_all_implementations.sh 1024 2048 4096
```

---

## 📊 Understanding Results

### Key Metrics

1. **Execution Time**: Wall-clock time in milliseconds
2. **GFLOPS**: Giga Floating-Point Operations Per Second
   - Formula: `(2 × n³) / (time_seconds × 10⁹)`
3. **Speedup**: `Time(sequential) / Time(parallel)`
4. **Efficiency**: `Speedup / Number_of_Processors`

### Expected Results

#### Strong Scaling (4096×4096)
```
Processors | Time (s) | GFLOPS | Speedup | Efficiency
-----------|----------|--------|---------|------------
1          | 120.5    | 1.14   | 1.00×   | 100%
2          | 62.3     | 2.20   | 1.93×   | 97%
4          | 33.1     | 4.15   | 3.64×   | 91%
8          | 18.5     | 7.42   | 6.51×   | 81%
16         | 11.2     | 12.26  | 10.75×  | 67%
```

**Good efficiency**: > 80% up to moderate processor counts

#### Weak Scaling
```
Processors | Size    | Time (s) | GFLOPS | Efficiency
-----------|---------|----------|--------|------------
1          | 2048    | 15.2     | 1.13   | 100%
2          | 4096    | 15.8     | 2.17   | 96%
4          | 8192    | 16.1     | 4.27   | 94%
8          | 16384   | 16.5     | 8.33   | 92%
```

**Good weak scaling**: Time remains nearly constant

---

## 🔍 Analyzing Results

### Quick Analysis Script

```bash
# Extract key metrics
./scripts/analyze_results.sh results/

# Or manually grep
grep -r "GFLOPS" results/ | sort
grep -r "Speedup" results/ | sort
```

### Create Visualizations

Use Python/Matplotlib to create plots:

```python
import matplotlib.pyplot as plt
import numpy as np

# Parse results and plot
# - Speedup vs Processors
# - Efficiency vs Processors
# - GFLOPS vs Problem Size
```

---

## 🐛 Troubleshooting

### Build Issues

**Problem**: Compiler not found
```bash
# Solution: Load modules or set paths
module load gcc
# or
export PATH=/path/to/gcc/bin:$PATH
```

**Problem**: MPI not found
```bash
# Solution: Load MPI module
module load openmpi
# or
which mpic++
```

**Problem**: OpenMP not working
```bash
# Check compiler flags
g++ -fopenmp --version
# Ensure Makefile includes -fopenmp
```

### Runtime Issues

**Problem**: "mpirun: command not found"
```bash
# Load MPI module
module load openmpi
# or check installation
which mpirun
```

**Problem**: "Cannot allocate memory"
```bash
# Reduce matrix size or request more memory
# In SLURM: #SBATCH --mem=64G
```

**Problem**: Job killed/timeout
```bash
# Increase time limit
#SBATCH --time=08:00:00  # 8 hours
```

**Problem**: Poor performance
```bash
# Check CPU affinity
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Check system load
htop
# Ensure exclusive nodes if possible
```

### MPI Issues

**Problem**: "Too many processes"
```bash
# Check available resources
sinfo  # SLURM
# Reduce number of processes
mpirun -np 4 ./bin/performance  # Instead of 64
```

**Problem**: Communication errors
```bash
# Check network configuration
# Try different MPI implementation
module swap openmpi intel-mpi
```

---

## 📝 Best Practices

1. **Start Small**: Test with small matrices first
2. **Use Exclusive Nodes**: Request `--exclusive` for consistent results
3. **Multiple Runs**: Average over 3-5 runs for statistical significance
4. **Monitor Resources**: Use `htop`, `nvidia-smi` (if GPU), etc.
5. **Save Everything**: Keep all output files for later analysis
6. **Document Environment**: Note compiler versions, module versions
7. **Check Limits**: Be aware of time/memory/job limits
8. **Off-Peak Hours**: Run during low-load periods for consistent results

---

## 🎯 Quick Reference Commands

```bash
# Build
make clean && make

# Quick test
export OMP_NUM_THREADS=8
./bin/performance

# MPI test
mpirun -np 4 ./bin/performance

# Strong scaling
./scripts/benchmark_strong_scaling.sh 4096

# Weak scaling
./scripts/benchmark_weak_scaling.sh 2048

# Comprehensive
./scripts/benchmark_all_implementations.sh

# Submit SLURM job
sbatch scripts/benchmark_slurm.sh

# Check job status (SLURM)
squeue -u $USER

# View results
./scripts/analyze_results.sh results/
```

---

## 📚 Additional Resources

- **SLURM Documentation**: https://slurm.schedmd.com/
- **OpenMPI Documentation**: https://www.open-mpi.org/
- **OpenMP Documentation**: https://www.openmp.org/
- **Your Supercomputer's Documentation**: Check your cluster's wiki/docs

---

## 💡 Tips for Your Assignment

1. **Strong Scaling**: Show how speedup changes with more processors
2. **Weak Scaling**: Show how performance scales with problem size
3. **Compare Implementations**: Naive vs Strassen vs OpenMP vs MPI vs Hybrid
4. **Efficiency Analysis**: Calculate and report parallel efficiency
5. **Visualizations**: Create graphs of speedup/efficiency vs processors
6. **Discussion**: Explain why efficiency decreases with more processors
7. **Communication Overhead**: Measure and report MPI communication time separately

---

**Good luck with your benchmarking!** 🚀

