# Compilers
CXX_SERIAL = g++-15
CXX_OMP = g++-15
CXX_MPI = mpicxx

N ?= 1000

EIGEN ?= /opt/homebrew/include/eigen3
OMP_NUM_THREADS ?= 8
MPI_NUM_PROC ?= 8

INCLUDES = -Iinclude -I$(EIGEN)
CXXFLAGS = -std=c++23 -Wall -Wextra $(INCLUDES) -O3 -ffast-math -funroll-loops -march=native
OMPFLAGS = -fopenmp

# Directories
OBJ_DIR = obj
BIN_DIR = bin

# Phony targets
.PHONY: all test clean run_test_serial run_test_omp run_test_mpi run_test_hybrid

# Default target
all: $(BIN_DIR)/test_serial $(BIN_DIR)/test_omp $(BIN_DIR)/test_mpi $(BIN_DIR)/test_hybrid

# Create directories
$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

# --- Object File Compilation ---

# Serial objects
$(OBJ_DIR)/multiply.o: src/multiply.cpp | $(OBJ_DIR)
	$(CXX_SERIAL) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/strassen.o: src/strassen.cpp | $(OBJ_DIR)
	$(CXX_SERIAL) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: src/utils.cpp | $(OBJ_DIR)
	$(CXX_SERIAL) $(CXXFLAGS) -c $< -o $@

# Test utility object
$(OBJ_DIR)/test_utils.o: tests/utils.cpp | $(OBJ_DIR)
	$(CXX_SERIAL) $(CXXFLAGS) -c $< -o $@

# OpenMP objects
$(OBJ_DIR)/multiply_openmp.o: src/multiply_openmp.cpp | $(OBJ_DIR)
	$(CXX_OMP) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

$(OBJ_DIR)/strassen_omp.o: src/strassen_omp.cpp | $(OBJ_DIR)
	$(CXX_OMP) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

# MPI objects
$(OBJ_DIR)/multiply_mpi.o: src/multiply_mpi.cpp | $(OBJ_DIR)
	$(CXX_MPI) $(CXXFLAGS) -c $< -o $@

# Hybrid objects
$(OBJ_DIR)/multiply_hybrid.o: src/multiply_hybrid.cpp | $(OBJ_DIR)
	$(CXX_MPI) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

#strassen objs
$(OBJ_DIR)/strassen_mpi.o: src/strassen_mpi.cpp | $(OBJ_DIR)
	$(CXX_MPI) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/strassen_hybrid.o: src/strassen_hybrid.cpp | $(OBJ_DIR)
	$(CXX_MPI) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@


# --- Test Executable Linking ---

# Dependencies
TEST_SERIAL_OBJS = $(OBJ_DIR)/multiply.o $(OBJ_DIR)/strassen.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/test_utils.o
TEST_OMP_OBJS = $(OBJ_DIR)/multiply_openmp.o $(OBJ_DIR)/strassen_omp.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/test_utils.o
TEST_MPI_OBJS = $(OBJ_DIR)/multiply_mpi.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/test_utils.o $(OBJ_DIR)/multiply.o
TEST_HYBRID_OBJS = $(OBJ_DIR)/multiply_hybrid.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/test_utils.o $(OBJ_DIR)/multiply_openmp.o
TEST_STRASSEN_OBJS = $(OBJ_DIR)/strassen_mpi.o $(OBJ_DIR)/strassen_hybrid.o  $(OBJ_DIR)/multiply_openmp.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/multiply.o $(OBJ_DIR)/test_utils.o

# Linking rules
$(BIN_DIR)/test_serial: tests/test_serial.cpp $(TEST_SERIAL_OBJS) | $(BIN_DIR)
	$(CXX_SERIAL) $(CXXFLAGS) -o $@ $< $(TEST_SERIAL_OBJS)

$(BIN_DIR)/test_omp: tests/test_omp.cpp $(TEST_OMP_OBJS) | $(BIN_DIR)
	$(CXX_OMP) $(CXXFLAGS) $(OMPFLAGS) -o $@ $< $(TEST_OMP_OBJS)

$(BIN_DIR)/test_mpi: tests/test_mpi.cpp $(TEST_MPI_OBJS) | $(BIN_DIR)
	$(CXX_MPI) $(CXXFLAGS) -o $@ $< $(TEST_MPI_OBJS)

$(BIN_DIR)/test_hybrid: tests/test_hybrid.cpp $(TEST_HYBRID_OBJS) | $(BIN_DIR)
	$(CXX_MPI) $(CXXFLAGS) $(OMPFLAGS) -o $@ $< $(TEST_HYBRID_OBJS)

$(BIN_DIR)/test_strassen: tests/test_strassen.cpp $(TEST_STRASSEN_OBJS) | $(BIN_DIR)
	$(CXX_MPI) $(CXXFLAGS) $(OMPFLAGS) -o $@ $< $(TEST_STRASSEN_OBJS)

# --- Run and Clean ---

test_serial: $(BIN_DIR)/test_serial
	@echo "Running serial test (N=$(N))..."
	./$< $(N)

test_omp: $(BIN_DIR)/test_omp
	@echo "Running OpenMP test (N=$(N))..."
	./$< $(N) OMP_NUM_THREADS=$(OMP_NUM_THREADS)

test_mpi: $(BIN_DIR)/test_mpi
	@echo "Running MPI test (N=$(N))..."
ifdef HOSTS
	mpirun -np $(MPI_NUM_PROC) -hosts $(HOSTS) ./$< $(N)
else
	mpirun -np $(MPI_NUM_PROC) ./$< $(N)
endif

test_hybrid: $(BIN_DIR)/test_hybrid
	@echo "Running hybrid test (N=$(N))..."
ifdef HOSTS
	mpirun -np $(MPI_NUM_PROC) -hosts $(HOSTS) ./$< $(N) OMP_NUM_THREADS=$(OMP_NUM_THREADS)
else
	mpirun -np $(MPI_NUM_PROC) ./$< $(N) OMP_NUM_THREADS=$(OMP_NUM_THREADS)
endif
 
test_strassen: $(BIN_DIR)/test_strassen
	@echo "Running Strassen tests (N=$(N))..."
ifdef HOSTS
	mpirun -np 7 -hosts $(HOSTS) ./$< $(N)
else
	mpirun -np 7 ./$< $(N)
endif

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)