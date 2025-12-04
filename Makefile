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
MATRIX_OBJS = obj/matrix.o obj/multiply_naive_sequential.o obj/multiply_openmp.o obj/multiply_strassen_sequential.o 
MPI_OBJS = obj/multiply_mpi.o obj/multiply_hybrid.o
ALL_OBJS = $(MATRIX_OBJS) $(MPI_OBJS)

# Default target: build both test executables
all: correctness performance

# MPI-specific target: build all test executables (including MPI versions)
all_mpi: correctness performance correctness_mpi performance_mpi

.PHONY: all all_mpi correctness performance correctness_mpi performance_mpi test test_mpi clean help

obj/matrix.o: src/matrix.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/matrix.cpp -o obj/matrix.o

obj/multiply_naive_sequential.o: src/multiply_naive_sequential.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/multiply_naive_sequential.cpp -o obj/multiply_naive_sequential.o

obj/multiply_strassen_sequential.o: src/multiply_strassen_sequential.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c src/multiply_strassen_sequential.cpp -o obj/multiply_strassen_sequential.o

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

correctness: tests/test_correctness.cpp tests/test_cases.h $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_correctness.cpp $(MATRIX_OBJS) -o bin/correctness $(LDFLAGS)

performance: tests/test_performance.cpp tests/benchmark_cases.h $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_performance.cpp $(MATRIX_OBJS) -o bin/performance $(LDFLAGS)

correctness_mpi: tests/test_correctness_mpi.cpp tests/test_cases.h $(ALL_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_correctness_mpi.cpp $(ALL_OBJS) -o bin/correctness_mpi $(LDFLAGS)

performance_mpi: tests/test_performance_mpi.cpp tests/benchmark_cases.h $(ALL_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_performance_mpi.cpp $(ALL_OBJS) -o bin/performance_mpi $(LDFLAGS)

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

# Run MPI tests (MPI and Hybrid implementations - both correctness and performance)
test_mpi: correctness_mpi performance_mpi
	@echo "========================================"
	@echo "Running MPI Correctness Tests..."
	@echo "========================================"
	mpirun -np 4 ./bin/correctness_mpi
	@echo ""
	@echo "========================================"
	@echo "Running MPI Performance Benchmarks..."
	@echo "========================================"
	mpirun -np 4 ./bin/performance_mpi

clean:
	@echo "Cleaning build artifacts..."
	@rm -rf obj bin
	@echo "Clean complete."

# Help target
help:
	@echo "Matrix Multiplication Project - Makefile Help"
	@echo ""
	@echo "Targets:"
	@echo "  make                    - Build non-MPI test executables (default)"
	@echo "  make all_mpi            - Build all test executables including MPI"
	@echo "  make correctness        - Build correctness test suite (Naive, OpenMP, Strassen)"
	@echo "  make performance        - Build performance benchmark suite (Naive, OpenMP, Strassen)"
	@echo "  make correctness_mpi    - Build MPI correctness test suite (MPI, Hybrid, Strassen variants)"
	@echo "  make performance_mpi    - Build MPI performance benchmark suite (MPI, Hybrid, Strassen variants)"
	@echo "  make test               - Run non-MPI tests (correctness + performance)"
	@echo "  make test_mpi           - Run MPI tests with 4 processes (correctness + performance)"
	@echo "  make clean              - Remove all build artifacts"
	@echo "  make help               - Show this help message"
	@echo ""
	@echo "Compilation Options:"
	@echo "  Edit OPTFLAGS in Makefile to change optimization level"
	@echo "  For debugging: Use -O0 -g instead of -O3 -march=native"
	@echo ""
	@echo "Examples:"
	@echo "  make clean && make                          # Clean build (non-MPI)"
	@echo "  make test                                   # Run non-MPI tests"
	@echo "  OMP_NUM_THREADS=8 make test                # Run with 8 OpenMP threads"
	@echo "  make test_mpi                               # Run MPI tests with 4 processes"
	@echo "  mpirun -np 8 ./bin/correctness_mpi         # Run MPI correctness with 8 processes"
	@echo "  mpirun -np 8 ./bin/performance_mpi 512 1024 # Run MPI benchmarks with custom sizes"