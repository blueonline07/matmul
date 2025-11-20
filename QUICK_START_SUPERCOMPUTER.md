# Quick Start: Running on Supercomputer

## 🚀 5-Minute Setup

### Step 1: Transfer Code
```bash
# On supercomputer
cd ~/workspace
git clone <your-repo> matmul
cd matmul
```

### Step 2: Load Modules
```bash
# Common modules (adjust for your system)
module load gcc/11.2.0
module load openmpi/4.1.0
```

### Step 3: Build
```bash
make clean && make
```

### Step 4: Quick Test
```bash
# Single node test
export OMP_NUM_THREADS=8
./bin/performance 1024

# MPI test (if allowed)
mpirun -np 4 ./bin/performance 1024
```

### Step 5: Run Benchmarks
```bash
# Make scripts executable
chmod +x scripts/*.sh

# Strong scaling
./scripts/benchmark_strong_scaling.sh 4096

# Or submit as job
sbatch scripts/benchmark_slurm.sh
```

---

## 📋 Essential Commands

### Direct Performance Test
```bash
# Custom sizes
./bin/performance 512 1024 2048 4096

# Default sizes
./bin/performance
```

### Using Scripts
```bash
# Strong scaling (fixed size, varying parallelism)
./scripts/benchmark_strong_scaling.sh 4096

# Weak scaling (size grows with parallelism)
./scripts/benchmark_weak_scaling.sh 2048

# All implementations
./scripts/benchmark_all_implementations.sh
```

### Job Submission (SLURM)
```bash
# Submit job
sbatch scripts/benchmark_slurm.sh

# Check status
squeue -u $USER

# View output
cat benchmark_<jobid>.out
```

---

## 📊 What Gets Tested

### Strong Scaling
- **Fixed**: Matrix size (e.g., 4096×4096)
- **Varies**: Number of processors (1, 2, 4, 8, 16, 32, 64)
- **Measures**: Speedup, efficiency, communication overhead

### Weak Scaling
- **Fixed**: Problem size per processor (e.g., 2048×2048)
- **Varies**: Total problem size (grows with processors)
- **Measures**: Scalability, overhead impact

### Comprehensive
- **Tests**: All implementations (Naive, Strassen, OpenMP, MPI, Hybrid)
- **Sizes**: Multiple matrix sizes
- **Measures**: Complete performance comparison

---

## 📁 Output Locations

All results are saved in `results/` directory:
```
results/
├── strong_scaling_4096/
│   ├── openmp_1threads.txt
│   ├── openmp_8threads.txt
│   ├── mpi_4procs.txt
│   └── ...
├── weak_scaling/
│   ├── mpi_4procs_8192size.txt
│   └── ...
└── comprehensive/
    ├── seq_2048.txt
    ├── openmp8_2048.txt
    └── ...
```

---

## 🔍 Analyzing Results

```bash
# Quick analysis
./scripts/analyze_results.sh results/

# Or manually
grep -r "GFLOPS" results/ | sort
grep -r "Speedup" results/ | sort
```

---

## ⚙️ Customization

### Edit Job Scripts
```bash
# Edit SLURM script
nano scripts/benchmark_slurm.sh

# Adjust:
# - --nodes: Number of nodes
# - --ntasks-per-node: MPI processes per node
# - --cpus-per-task: OpenMP threads per process
# - --time: Maximum runtime
# - --partition: Your cluster's partition name
```

### Adjust Matrix Sizes
```bash
# In scripts, or pass as arguments
./scripts/benchmark_all_implementations.sh 1024 2048 4096 8192
```

---

## 🐛 Common Issues

### "mpirun not found"
```bash
module load openmpi
```

### "Out of memory"
```bash
# Reduce matrix size
./bin/performance 2048  # Instead of 4096
```

### "Job timeout"
```bash
# Increase time in job script
#SBATCH --time=08:00:00
```

### "Permission denied"
```bash
chmod +x scripts/*.sh
```

---

## 📚 Full Documentation

- **SUPERCOMPUTER_GUIDE.md**: Complete guide
- **scripts/README.md**: Script documentation
- **README.md**: Project overview

---

## 💡 Pro Tips

1. **Start Small**: Test with 512×512 first
2. **Check Resources**: `sinfo` (SLURM) or `qstat` (PBS)
3. **Use Exclusive Nodes**: `--exclusive` flag for consistent results
4. **Multiple Runs**: Average 3-5 runs for accuracy
5. **Monitor**: Use `htop` during benchmarks
6. **Save Everything**: Keep all output files

---

**Ready to benchmark!** 🎯

