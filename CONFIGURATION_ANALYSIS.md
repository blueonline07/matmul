# System Configuration Analysis

## Your Supercomputer Specifications

### CPU Configuration
- **Model**: Intel Xeon E5-2680 v3 @ 2.50GHz
- **Total CPUs**: 8 physical cores
- **Sockets**: 2
- **Cores per socket**: 4
- **Threads per core**: 1 (no hyperthreading)
- **NUMA nodes**: 2
  - NUMA node0: CPUs 0-3
  - NUMA node1: CPUs 4-7

### Memory Configuration
- **Total RAM**: 15 GiB (~15.4 GB)
- **Available**: 15 GiB
- **Swap**: None (0 B)

### Software
- **MPI**: OpenMPI 4.1.2
- **Architecture**: x86_64
- **Virtualization**: KVM (running in VM)

### Cache Hierarchy
- **L1d cache**: 256 KiB (8 instances) = 32 KiB per core
- **L1i cache**: 256 KiB (8 instances) = 32 KiB per core
- **L2 cache**: 32 MiB (8 instances) = 4 MiB per core
- **L3 cache**: 32 MiB (2 instances) = 16 MiB per socket

---

## 🎯 Recommended Benchmark Configuration

### Matrix Size Limits

**Memory Calculation:**
- Each matrix: `size × size × 8 bytes` (double precision)
- For multiplication: Need 3 matrices (A, B, C)
- Total: `3 × size² × 8 bytes`

**Maximum Safe Sizes:**
```
15 GB total memory
- OS overhead: ~1 GB
- Available: ~14 GB
- Per matrix: ~4.67 GB max
- Max size: sqrt(4.67 GB / 8 bytes) ≈ 24,000

Recommended sizes:
- Small:  512, 1024, 2048
- Medium: 4096, 8192
- Large:  12288, 16384 (if needed)
- Max:    20480 (may be tight)
```

### Recommended Test Sizes
```bash
# Conservative (safe)
512 1024 2048 4096

# Aggressive (if you have exclusive access)
512 1024 2048 4096 8192 12288
```

---

## ⚙️ Optimal Parallel Configuration

### OpenMP (Single Node)
```bash
# Use all 8 cores
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# NUMA-aware: bind threads to cores
export OMP_PROC_BIND=close
```

### MPI Configuration
```bash
# For 2 NUMA nodes, optimal is:
# - 2 MPI processes (1 per NUMA node)
# - 4 OpenMP threads per process

# Or pure MPI:
mpirun -np 8 ./bin/performance  # 1 process per core
```

### Hybrid Configuration (Recommended for NUMA)
```bash
# 2 MPI processes × 4 OpenMP threads = 8 total
export OMP_NUM_THREADS=4
mpirun -np 2 ./bin/performance

# This respects NUMA topology:
# - Process 0 on NUMA node0 (CPUs 0-3)
# - Process 1 on NUMA node1 (CPUs 4-7)
```

---

## 📊 Expected Performance Characteristics

### Cache Behavior
- **L1**: 32 KiB per core - Good for matrices up to ~64×64
- **L2**: 4 MiB per core - Good for matrices up to ~700×700
- **L3**: 16 MiB per socket - Good for matrices up to ~1400×1400
- **RAM**: 15 GB - Can handle up to ~43,000×43,000 (theoretically)

### NUMA Considerations
- **2 NUMA nodes**: Memory access patterns matter
- **Best performance**: Keep data local to NUMA node
- **Hybrid approach**: 2 MPI processes (1 per node) + 4 threads each

### Expected Speedups
```
Sequential baseline: 1.0×
OpenMP (8 threads): 6-7× (good efficiency)
MPI (8 processes): 5-6× (communication overhead)
Hybrid (2×4): 6-7× (best for NUMA)
```

---

## 🔧 Recommended Benchmark Scripts

### Script 1: NUMA-Optimized Strong Scaling
```bash
#!/bin/bash
# Strong scaling with NUMA awareness

MATRIX_SIZE=4096
export OMP_PLACES=cores
export OMP_PROC_BIND=close

# Pure OpenMP
for threads in 1 2 4 8; do
    export OMP_NUM_THREADS=$threads
    ./bin/performance $MATRIX_SIZE
done

# Pure MPI
for procs in 1 2 4 8; do
    mpirun -np $procs ./bin/performance $MATRIX_SIZE
done

# Hybrid (NUMA-optimized)
for nodes in 1 2; do
    threads_per_node=4
    export OMP_NUM_THREADS=$threads_per_node
    mpirun -np $nodes --bind-to socket ./bin/performance $MATRIX_SIZE
done
```

### Script 2: Memory-Aware Sizing
```bash
#!/bin/bash
# Test sizes that fit in memory

# Conservative sizes
SIZES=(512 1024 2048 4096)

# For each size, test different configurations
for size in "${SIZES[@]}"; do
    echo "Testing ${size}×${size}"
    
    # Sequential
    export OMP_NUM_THREADS=1
    ./bin/performance $size
    
    # OpenMP (8 threads)
    export OMP_NUM_THREADS=8
    ./bin/performance $size
    
    # Hybrid (2×4 - NUMA optimized)
    export OMP_NUM_THREADS=4
    mpirun -np 2 --bind-to socket ./bin/performance $size
done
```

---

## 🎯 Specific Recommendations

### 1. Matrix Sizes to Test
```bash
# Start with these (safe, good coverage)
./bin/performance 512 1024 2048 4096

# If you have exclusive access, add:
./bin/performance 512 1024 2048 4096 8192
```

### 2. OpenMP Configuration
```bash
# Always set these for best performance
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close
export OMP_PROC_BIND=spread  # Alternative: spread across sockets
```

### 3. MPI Configuration
```bash
# NUMA-aware binding
mpirun -np 2 --bind-to socket --map-by socket ./bin/performance

# Or let OpenMPI auto-detect
mpirun -np 8 ./bin/performance
```

### 4. Hybrid Configuration (Best for Your System)
```bash
# 2 MPI processes (1 per NUMA node) × 4 OpenMP threads
export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=close

mpirun -np 2 \
    --bind-to socket \
    --map-by socket \
    ./bin/performance 4096
```

---

## 📈 Expected Results

### Strong Scaling (4096×4096)
```
Threads/Procs | Time (s) | GFLOPS | Speedup | Efficiency
--------------|----------|--------|---------|------------
1             | ~120     | ~1.1   | 1.00×   | 100%
2             | ~62      | ~2.2   | 1.94×   | 97%
4             | ~33      | ~4.1   | 3.64×   | 91%
8 (OpenMP)    | ~18      | ~7.5   | 6.67×   | 83%
8 (MPI)       | ~22      | ~6.2   | 5.45×   | 68%
2×4 (Hybrid)  | ~17      | ~8.0   | 7.06×   | 88%
```

### Weak Scaling (2048 per processor)
```
Total Size    | Processors | Time (s) | GFLOPS | Efficiency
--------------|------------|----------|--------|------------
2048×2048     | 1          | ~15      | ~1.1   | 100%
4096×4096     | 2          | ~16      | ~2.1   | 95%
8192×8192    | 4          | ~17      | ~4.0   | 91%
16384×16384  | 8          | ~19      | ~7.2   | 81%
```

---

## ⚠️ Important Considerations

### Memory Limits
- **15 GB total**: Be careful with large matrices
- **No swap**: Out of memory = crash
- **Recommendation**: Don't exceed 8192×8192 without checking

### NUMA Effects
- **2 NUMA nodes**: Memory access patterns matter
- **Best**: Hybrid approach (2 MPI × 4 threads)
- **Worst**: Pure MPI with random process placement

### Virtualization Overhead
- **KVM**: Some overhead expected
- **Performance**: May be 5-10% slower than bare metal
- **Still valid**: Results are still meaningful for comparison

### Cache Optimization
- **L3 cache**: 16 MiB per socket
- **Optimal**: Matrices that fit in L3 (up to ~1400×1400)
- **Larger**: Will see memory bandwidth limits

---

## 🚀 Quick Start Commands

### Test 1: Quick Single-Node
```bash
export OMP_NUM_THREADS=8
export OMP_PLACES=cores
export OMP_PROC_BIND=close
./bin/performance 1024 2048 4096
```

### Test 2: NUMA-Optimized Hybrid
```bash
export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=close
mpirun -np 2 --bind-to socket ./bin/performance 4096
```

### Test 3: Strong Scaling
```bash
./scripts/benchmark_strong_scaling.sh 4096
```

### Test 4: Memory Stress Test
```bash
# Test largest safe size
./bin/performance 8192
```

---

## 📝 Customized SLURM Script

For your system, here's an optimized SLURM script:

```bash
#!/bin/bash
#SBATCH --job-name=matmul_numa
#SBATCH --output=benchmark_%j.out
#SBATCH --time=02:00:00
#SBATCH --nodes=1          # Single node (you have 8 cores)
#SBATCH --ntasks-per-node=2  # 2 MPI processes (1 per NUMA node)
#SBATCH --cpus-per-task=4    # 4 OpenMP threads per MPI process
#SBATCH --mem=14G            # Leave 1GB for OS
#SBATCH --exclusive

module load gcc openmpi

export OMP_NUM_THREADS=4
export OMP_PLACES=cores
export OMP_PROC_BIND=close

cd $SLURM_SUBMIT_DIR
make clean && make

# NUMA-optimized hybrid
mpirun -np 2 --bind-to socket --map-by socket \
    ./bin/performance 1024 2048 4096 8192
```

---

## 🎓 Key Takeaways

1. **8 physical cores, no hyperthreading**: Max 8-way parallelism
2. **2 NUMA nodes**: Hybrid (2×4) likely best configuration
3. **15 GB RAM**: Safe up to ~8192×8192, max ~16384×16384
4. **NUMA-aware**: Use `--bind-to socket` for MPI
5. **Cache-friendly**: Matrices < 1400×1400 benefit from L3 cache

---

**Your system is well-suited for benchmarking!** The NUMA topology makes it interesting for studying hybrid MPI+OpenMP performance.

