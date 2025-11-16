# Matrix Multiplication Assignment

This project structure is designed for implementing and comparing various matrix multiplication algorithms as per the assignment requirements.

## Assignment Requirements

You are required to implement:
1. **Naive matrix multiplication** - Your own implementation
2. **Strassen's algorithm** - Divide and conquer approach
3. **OpenMP parallelization** - For shared-memory platforms (laptops/notebooks)
4. **MPI parallelization** - For distributed-memory platforms (clusters)
5. **Hybrid MPI+OpenMP** - Marriage of MPI processes and OpenMP threads

Each implementation must:
- Be tested for correctness
- Be benchmarked for performance (100×100 to 10,000×10,000)
- Be compared with existing matrix multiplication libraries

## Project Structure

```
matmul/
├── Makefile                    # Build configuration with Make
├── README.md                   # This file
├── .gitignore                  # Git ignore rules
├── include/                    # Header files (YOU IMPLEMENT)
│   ├── matrix.h               # Matrix structure and function prototypes
│   └── utils.h                # Utility functions (timing, validation, etc.)
├── src/                       # Implementation files (YOU IMPLEMENT)
│   ├── naive_matmul.c         # Naive O(n³) implementation
│   ├── strassen_matmul.c      # Strassen's O(n^2.807) algorithm
│   ├── openmp_matmul.c        # OpenMP shared-memory parallel version
│   ├── mpi_matmul.c           # MPI distributed-memory parallel version
│   ├── hybrid_matmul.c        # Hybrid MPI+OpenMP implementation
│   └── utils.c                # Utility functions implementation
├── tests/                     # Testing and benchmarking (YOU IMPLEMENT)
│   ├── test_correctness.c     # Validate correctness of implementations
│   └── benchmark.c            # Performance benchmarking suite
├── scripts/                   # Automation scripts
│   └── run_all_benchmarks.sh  # Script to run comprehensive benchmarks
├── bin/                       # Compiled executables (created by make)
├── obj/                       # Object files (created by make)
└── results/                   # Benchmark results (created during testing)
```

## Getting Started

### Prerequisites

- **C/C++ Compiler**: GCC with OpenMP support
- **MPI Implementation**: OpenMPI or MPICH
- **Make**: Build automation tool

On macOS:
```bash
# Install GCC with OpenMP
brew install gcc

# Install OpenMPI
brew install open-mpi
```

On Linux:
```bash
# Ubuntu/Debian
sudo apt-get install gcc make libopenmpi-dev

# CentOS/RHEL
sudo yum install gcc make openmpi-devel
```

### Building the Project

The Makefile is already configured. Once you implement your code:

```bash
# Build all implementations
make

# Build specific components
make bin/naive
make bin/strassen
make bin/openmp
make bin/mpi
make bin/hybrid

# Clean build artifacts
make clean

# Clean everything including results
make cleanall
```

## Implementation Guidelines

### 1. Matrix Structure (`include/matrix.h`)

Define your matrix structure and API:
- Matrix data structure (consider row-major vs column-major)
- Creation/destruction functions
- Matrix operations (add, subtract, multiply)
- Helper functions (print, compare, validate)

### 2. Utility Functions (`include/utils.h`, `src/utils.c`)

Implement utilities for:
- **Timing**: High-resolution timing for benchmarking
- **Initialization**: Random, identity, constant matrices
- **Validation**: Compare results, check dimensions
- **Performance metrics**: Calculate GFLOPS

### 3. Algorithm Implementations

#### Naive Implementation (`src/naive_matmul.c`)
- Standard triple-nested loop
- Complexity: O(n³)
- Use as baseline for correctness and performance

#### Strassen's Algorithm (`src/strassen_matmul.c`)
- Recursive divide-and-conquer
- 7 multiplications instead of 8
- Base case threshold (switch to naive for small matrices)
- Handle non-power-of-2 sizes (padding)
- Complexity: O(n^2.807)

#### OpenMP Implementation (`src/openmp_matmul.c`)
- Parallelize with `#pragma omp parallel for`
- Consider loop collapse and scheduling strategies
- Test with varying thread counts

#### MPI Implementation (`src/mpi_matmul.c`)
- Design data distribution strategy (row-wise, column-wise, block)
- Handle communication (broadcast, scatter, gather)
- Consider load balancing

#### Hybrid Implementation (`src/hybrid_matmul.c`)
- Combine MPI for inter-node and OpenMP for intra-node parallelism
- Use `MPI_Init_thread` with appropriate thread level
- Balance MPI processes and OpenMP threads

### 4. Testing (`tests/test_correctness.c`)

Devise testing approaches:
- Compare against naive implementation
- Test with known results (identity matrix, etc.)
- Verify with small matrices (easy to check manually)
- Test edge cases (different sizes, non-square matrices)

### 5. Benchmarking (`tests/benchmark.c`)

Conduct performance study:
- Matrix sizes: 100×100, 1,000×1,000, up to 10,000×10,000
- Measure execution time
- Calculate GFLOPS: `(2 × n³) / (time × 10⁹)`
- Vary parameters (threads, processes)
- Generate data for plotting (CSV format)
- Compare with libraries (BLAS, OpenBLAS, MKL)

## Running the Programs

### Individual Executables

```bash
# Naive implementation
./bin/naive [matrix_size]

# Strassen implementation
./bin/strassen [matrix_size]

# OpenMP implementation
./bin/openmp [matrix_size] [num_threads]

# MPI implementation
mpirun -np [num_processes] ./bin/mpi [matrix_size]

# Hybrid MPI+OpenMP
mpirun -np [num_processes] ./bin/hybrid [matrix_size] [num_threads]
```

### Testing Suite

```bash
make test
```

### Benchmark Suite

```bash
make benchmark

# Or run comprehensive benchmarks
chmod +x scripts/run_all_benchmarks.sh
./scripts/run_all_benchmarks.sh
```

## Performance Analysis

### Metrics to Collect

1. **Execution time** vs matrix size
2. **Speedup** = Time(serial) / Time(parallel)
3. **Efficiency** = Speedup / Number of processors
4. **GFLOPS** = (2 × n³) / (time × 10⁹)
5. **Scalability**: Strong scaling and weak scaling

### Comparison Points

- Compare all your implementations against each other
- Compare against existing libraries:
  - BLAS (Basic Linear Algebra Subprograms)
  - OpenBLAS
  - Intel MKL (Math Kernel Library)
  - ATLAS

### Visualization

Generate plots for:
- Execution time vs matrix size
- Speedup vs number of processors/threads
- Efficiency vs number of processors/threads
- GFLOPS comparison across implementations

## Tips and Best Practices

1. **Start small**: Implement and test with small matrices first
2. **Verify correctness**: Always validate before benchmarking
3. **Optimize compilation**: Use `-O3` flag for production runs
4. **Cache optimization**: Consider blocking/tiling for better cache usage
5. **Memory alignment**: Align memory for better vectorization
6. **Profiling**: Use tools like `gprof`, `perf`, or `Instruments` (macOS)
7. **Cluster testing**: Test MPI on actual cluster before large-scale runs

## Additional Considerations

### Strassen Algorithm
- Overhead significant for small matrices
- May use more memory than naive
- Numerical stability considerations

### OpenMP
- Thread affinity and binding
- False sharing in cache lines
- Optimal number of threads (typically = number of cores)

### MPI
- Communication overhead
- Load balancing
- Network latency and bandwidth

### Hybrid
- Balance between MPI processes and OpenMP threads
- Typically: fewer MPI processes × more OpenMP threads
- Example: 4 nodes × 8 cores = 4 MPI processes × 8 threads

## Resources

- **Strassen Algorithm**: "Gaussian Elimination is not Optimal" (1969)
- **OpenMP**: https://www.openmp.org/
- **MPI**: https://www.mpi-forum.org/
- **BLAS**: http://www.netlib.org/blas/
- **Performance Analysis**: "Introduction to High Performance Computing for Scientists and Engineers"

## License

This is an academic assignment project.
