# Matrix Multiplication Project

High-performance matrix multiplication implementations using various algorithms and parallelization techniques.

## Features

Implemented algorithms:
1. **Naive matrix multiplication** - Standard O(n³) triple-loop implementation
2. **Strassen's algorithm** - Divide and conquer approach with O(n^2.807) complexity
3. **OpenMP parallelization** - Shared-memory parallelism for multi-core systems
4. **MPI parallelization** - Distributed-memory parallelism for clusters
5. **Hybrid MPI+OpenMP** - Combined approach for maximum performance

Each implementation includes:
- Correctness validation tests
- Performance benchmarking (100×100 to 1,000×1,000+)
- Comparative analysis

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

## Optimization Tips

1. **Compiler flags**: Use `-O3` for aggressive optimization
2. **Cache efficiency**: Contiguous memory access (row-major ordering)
3. **Thread count**: Set to number of physical cores for OpenMP
4. **MPI processes**: Balance communication vs computation overhead
5. **Matrix blocking**: Consider tiling for better cache usage (future improvement)
6. **Strassen threshold**: Tune base case size for optimal performance

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
