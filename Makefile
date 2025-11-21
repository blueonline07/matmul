# Makefile for High-Performance Matrix Multiplication Project
#
# Compilers:
#   - CXX: Standard C++ compiler (GCC 15 with C++17 support)
#   - MPICXX: MPI C++ wrapper compiler (wraps CXX with MPI flags)
#
# Optimization flags:
#   -O3: Aggressive optimization (inlining, vectorization, loop unrolling)
#   -march=native: Optimize for current CPU architecture (enables AVX/AVX2/AVX-512)
#   -mtune=native: Fine-tune for current CPU
#   -ffast-math: Fast floating-point math (relaxes IEEE 754 for speed)
#   -funroll-loops: Aggressive loop unrolling
#   -flto: Link-time optimization across compilation units
#
# Warning flags:
#   -Wall: Enable most warnings
#   -Wextra: Additional warnings
#   -Wpedantic: Strict ISO C++ compliance warnings

CXX = g++-15
MPICXX = mpic++

# Base flags for all compilations
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# Optimization flags (comment out for debugging)
# Note: -march=native may not work on all platforms (e.g., Apple Silicon)
# For Apple M1/M2: GCC may need -mcpu=native instead of -march=native
# For maximum portability, you can remove architecture-specific flags
OPTFLAGS = -O3 -ffast-math -funroll-loops

# Debug flags (uncomment for debugging, comment out OPTFLAGS)
# OPTFLAGS = -O0 -g -fsanitize=address -fsanitize=undefined

# Combine flags
CXXFLAGS += $(OPTFLAGS)

LDFLAGS = 

# Object files
MATRIX_OBJS = obj/matrix.o obj/multiply_naive.o obj/multiply_openmp.o obj/multiply_mpi.o obj/multiply_strassen.o obj/multiply_hybrid.o

# Default target: build both test executables
all: correctness performance

.PHONY: all correctness performance test clean help

obj/matrix.o: src/matrix.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/matrix.cpp -o obj/matrix.o

obj/multiply_naive.o: src/multiply_naive.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/multiply_naive.cpp -o obj/multiply_naive.o

obj/multiply_strassen.o: src/multiply_strassen.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/multiply_strassen.cpp -o obj/multiply_strassen.o

obj/multiply_openmp.o: src/multiply_openmp.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -fopenmp -c src/multiply_openmp.cpp -o obj/multiply_openmp.o

obj/multiply_mpi.o: src/multiply_mpi.cpp
	@mkdir -p obj
	$(MPICXX) $(CXXFLAGS) -c src/multiply_mpi.cpp -o obj/multiply_mpi.o

# NEW RULE FOR HYBRID (MPI + OpenMP)
obj/multiply_hybrid.o: src/multiply_hybrid.cpp
	@mkdir -p obj
	$(MPICXX) $(CXXFLAGS) -fopenmp -c src/multiply_hybrid.cpp -o obj/multiply_hybrid.o

correctness: $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_correctness.cpp $(MATRIX_OBJS) -o bin/correctness $(LDFLAGS)

performance: $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_performance.cpp $(MATRIX_OBJS) -o bin/performance $(LDFLAGS)

test: correctness performance
	@echo "========================================"
	@echo "Running Correctness Tests..."
	@echo "========================================"
	./bin/correctness
	@echo ""
	@echo "========================================"
	@echo "Running Performance Benchmarks..."
	@echo "========================================"
	./bin/performance

# Run tests with MPI (requires mpirun)
test_mpi: correctness performance
	@echo "========================================"
	@echo "Running MPI Tests (4 processes)..."
	@echo "========================================"
	mpirun -np 4 ./bin/correctness
	mpirun -np 4 ./bin/performance

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf obj bin
	@echo "Clean complete."

# Help target
help:
	@echo "Matrix Multiplication Project - Makefile Help"
	@echo ""
	@echo "Targets:"
	@echo "  make               - Build all executables (default)"
	@echo "  make correctness   - Build correctness test suite"
	@echo "  make performance   - Build performance benchmark suite"
	@echo "  make test          - Build and run all tests"
	@echo "  make test_mpi      - Run tests with MPI (4 processes)"
	@echo "  make clean         - Remove all build artifacts"
	@echo "  make help          - Show this help message"
	@echo ""
	@echo "Compilation Options:"
	@echo "  Edit OPTFLAGS in Makefile to change optimization level"
	@echo "  For debugging: Use -O0 -g instead of -O3 -march=native"
	@echo ""
	@echo "Examples:"
	@echo "  make clean && make                    # Clean build"
	@echo "  make test                             # Run sequential tests"
	@echo "  OMP_NUM_THREADS=8 make test          # Run with 8 OpenMP threads"
	@echo "  mpirun -np 4 ./bin/performance       # Run MPI with 4 processes"
