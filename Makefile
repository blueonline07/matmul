# Simple Makefile for Matrix Multiplication Project (GNU libstdc++ build)

CXX = g++-15
MPICXX = mpic++
CXXFLAGS = -std=c++17 -O3 -Wall -Iinclude
LDFLAGS = 

# Source files
MATRIX_OBJS = obj/matrix.o obj/multiply_naive.o obj/multiply_openmp.o obj/multiply_mpi.o obj/multiply_strassen.o

all: correctness performance

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

correctness: $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_correctness.cpp $(MATRIX_OBJS) -o bin/correctness $(LDFLAGS)

performance: $(MATRIX_OBJS)
	@mkdir -p bin
	$(MPICXX) $(CXXFLAGS) -fopenmp tests/test_performance.cpp $(MATRIX_OBJS) -o bin/performance $(LDFLAGS)

test: correctness performance
	@echo "Running correctness tests..."
	./bin/correctness
	@echo "\nRunning performance tests..."
	./bin/performance

clean:
	rm -rf obj bin

.PHONY: all correctness performance  run_mult_mpi test tdd clean