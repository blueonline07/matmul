# Matrix Multiplication Project

High-performance matrix multiplication implementations using various algorithms and parallelization techniques.

## Features

### Implemented Algorithms

1. **Naive Matrix Multiplication** ✅
   - Optimized ikj loop ordering for cache efficiency
   - 2-4× faster than standard ijk ordering
   - Serves as baseline for performance comparisons

2. **Strassen's Algorithm** ✅
   - Divide-and-conquer with O(n^2.807) complexity
   - Automatically handles non-power-of-2 matrices
   - Tuned threshold (128) for optimal performance
   - Best for large matrices (n > 512)

3. **OpenMP Parallelization** ✅
   - Shared-memory parallelism for multi-core CPUs
   - Optimized with ikj ordering + dynamic scheduling
   - Near-linear speedup up to physical core count
   - Minimal thread synchronization overhead

4. **MPI Parallelization** ⚠️ TODO
   - Distributed-memory parallelism for clusters
   - Placeholder implementation (falls back to naive)
   - See `src/multiply_mpi.cpp` for implementation guide

5. **Hybrid MPI+OpenMP** ⚠️ TODO
   - Combined inter-node (MPI) + intra-node (OpenMP) parallelism
   - Optimal for multi-node clusters with multi-core nodes
   - See `src/multiply_mpi.cpp` for implementation guide

### Testing Framework

- **Correctness Tests**: Validates all implementations against known results
- **Performance Benchmarks**: Measures time, GFLOPS, and speedup
- **Scalability Analysis**: Tests multiple matrix sizes (100×100 to 2000×2000)

## Project Structure

```
matmul/
├── Makefile                      # Build configuration
├── README.md                     # This file
├── .gitignore                    # Git ignore rules
├── include/                      # Header files
│   └── matrix.h                  # Matrix class and API
├── src/                          # Implementation files
│   ├── matrix.cpp                # Matrix class implementation
│   ├── multiply_naive.cpp        # Naive O(n³) implementation
│   ├── multiply_strassen.cpp     # Strassen's algorithm
│   ├── multiply_openmp.cpp       # OpenMP parallelized version
│   ├── multiply_mpi.cpp          # MPI distributed version
│   └── multiply_hybrid.cpp       # Hybrid MPI+OpenMP (optional)
├── tests/                        # Testing and benchmarking
│   ├── test_correctness.cpp      # Correctness validation suite
│   └── test_performance.cpp      # Performance benchmarking suite
├── bin/                          # Compiled executables (created by make)
│   ├── correctness               # Correctness test binary
│   └── performance               # Performance test binary
└── obj/                          # Object files (created by make)
```

## Getting Started

## Prerequisites

- **C++ Compiler**: GCC 15+ with C++17 support
- **OpenMP**: For shared-memory parallelism
- **MPI**: OpenMPI or MPICH for distributed computing
- **Make**: Build automation

### Installation on macOS

```bash
# Install GCC with OpenMP support
brew install gcc@15

# Install OpenMPI
brew install open-mpi
```

### Installation on Linux

```bash
# Ubuntu/Debian
sudo apt-get install g++ make libopenmpi-dev

# CentOS/RHEL
sudo yum install gcc-c++ make openmpi-devel
```

## Building the Project

```bash
# Build all executables
make

# Build specific targets
make correctness      # Build correctness test suite
make performance      # Build performance benchmark suite

# Clean build artifacts
make clean
```

## Implementation Details

### Matrix Class (`include/matrix.h`, `src/matrix.cpp`)

The `Matrix` class provides:
- Contiguous memory storage (flat vector) for cache efficiency
- Row-major ordering: `data[i * cols + j]` for element at (i, j)
- Constructors, copy/move semantics
- Element access operators
- Initialization methods (random, identity, zeros, constant)
- Arithmetic operators (+, -, scalar multiplication)
- Utility methods (comparison, submatrix extraction, padding)

### Algorithms

#### Naive Implementation (`src/multiply_naive.cpp`)
- Standard triple-nested loop: O(n³)
- Used as baseline for correctness and performance comparison

#### Strassen's Algorithm (`src/multiply_strassen.cpp`)
- Recursive divide-and-conquer approach
- 7 multiplications instead of 8: O(n^2.807)
- Automatically pads non-power-of-2 matrices
- Base case threshold for small matrices

#### OpenMP Implementation (`src/multiply_openmp.cpp`)
- Parallelizes outer loops with `#pragma omp parallel for`
- Automatic thread scheduling
- Shared-memory parallelism for multi-core CPUs

#### MPI Implementation (`src/multiply_mpi.cpp`)
- Row-wise data distribution strategy
- Broadcast matrix B to all processes
- Scatter rows of matrix A across processes
- Gather results back to root process

#### Hybrid Implementation (`src/multiply_hybrid.cpp`)
- Combines MPI (inter-process) + OpenMP (intra-process)
- Uses `MPI_Init_thread` with thread support
- Optimal for multi-node clusters with multi-core nodes

## Running Tests

### Run All Tests

```bash
# Run both correctness and performance tests
make test
```

This will:
1. Build both test executables if needed
2. Run correctness tests (validates all implementations)
3. Run performance benchmarks (measures speed and GFLOPS)

### Run Individual Tests

```bash
# Run only correctness tests
./bin/correctness

# Run only performance benchmarks
./bin/performance
```

### MPI Tests

```bash
# Run with MPI (4 processes)
mpirun -np 4 ./bin/correctness
mpirun -np 4 ./bin/performance
```

## Performance Analysis

### Metrics

1. **Execution time** - Wall-clock time in milliseconds
2. **GFLOPS** - Giga floating-point operations per second: `(2 × n³) / (time × 10⁹)`
3. **Speedup** - Ratio vs naive implementation: `Time(naive) / Time(parallel)`
4. **Efficiency** - Speedup / Number of processors

### Expected Results

- **Naive**: Baseline performance, O(n³) complexity
- **Strassen**: Better for large matrices (n > 512), worse for small due to overhead
- **OpenMP**: Near-linear speedup up to number of physical cores
- **MPI**: Scales with number of processes, limited by communication overhead
- **Hybrid**: Best performance on multi-node clusters

## Testing Framework

### Correctness Tests (`tests/test_correctness.cpp`)

Validates all implementations against known results:
- 2×2 matrix multiplication
- Identity matrix multiplication (A × I = A)
- Zero matrix multiplication (A × 0 = 0)
- Non-square matrices (2×3 × 3×2)
- Associativity: (A × B) × C = A × (B × C)
- Cross-implementation consistency

### Performance Benchmarks (`tests/test_performance.cpp`)

Measures execution time and GFLOPS for:
- Matrix sizes: 100, 200, 500, 1000, etc.
- Calculates GFLOPS: `(2 × n³) / (time × 10⁹)`
- Computes speedup vs naive implementation
- Outputs formatted results table

## Performance Optimizations Implemented

### 1. **Cache-Optimized Loop Ordering**
- **ikj ordering** instead of standard ijk
- Sequential memory access for better cache locality
- ~2-4× speedup on large matrices

### 2. **Aggressive Compiler Optimizations**
```bash
-O3                 # Maximum optimization
-march=native       # CPU-specific instructions (AVX, AVX2, AVX-512)
-mtune=native       # Fine-tune for current CPU
-ffast-math         # Fast floating-point operations
-funroll-loops      # Loop unrolling for better pipelining
```

### 3. **OpenMP Thread Optimization**
- Parallelize outer loop only (minimize overhead)
- Dynamic scheduling for load balancing
- Chunk size tuned for cache efficiency

### 4. **Strassen Algorithm Tuning**
- Base case threshold: 128 (empirically determined)
- Switches to optimized naive for small submatrices
- Minimizes recursive overhead

### 5. **Matrix Storage**
- Contiguous memory (flat array) for cache-friendliness
- Row-major ordering matches C++ memory layout
- Direct pointer access for performance-critical code

## Additional Optimization Opportunities

For further performance improvements, consider:

1. **Cache Blocking/Tiling**: Divide matrices into cache-sized blocks
2. **SIMD Vectorization**: Explicit vector intrinsics (AVX-512)
3. **Prefetching**: Manual prefetch instructions for large matrices
4. **BLAS Integration**: Compare with optimized libraries (OpenBLAS, MKL)
5. **GPU Acceleration**: CUDA/OpenCL for massive parallelism

## Development Environment

- **Language**: C++17
- **Build System**: Make
- **Compiler**: GCC 15 (`g++-15`)
- **MPI Compiler**: `mpic++`
- **Parallelization**: OpenMP (`-fopenmp`), MPI

## References

- Strassen, V. (1969). "Gaussian Elimination is not Optimal"
- OpenMP: https://www.openmp.org/
- MPI Forum: https://www.mpi-forum.org/
- BLAS Reference: http://www.netlib.org/blas/

## License

Academic project for educational purposes.
