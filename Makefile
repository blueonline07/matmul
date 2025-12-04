CXX = mpic++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -fopenmp

# All source files
SRC_FILES = $(wildcard src/*.cpp)
# All test files
TEST_FILES = $(wildcard tests/*.cpp)

# Object files for main sources
OBJ_FILES = $(patsubst src/%.cpp, obj/%.o, $(filter-out src/main.cpp, $(SRC_FILES)))

# Test executables
TEST_EXECS = $(patsubst tests/%.cpp, bin/%, $(TEST_FILES))

# Phony targets
.PHONY: all run_tests clean

all: $(TEST_EXECS)

# Generic rule to build test executables
bin/%: tests/%.cpp $(OBJ_FILES)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o $@ $< $(filter-out obj/main.o,$(OBJ_FILES))

# Rule to build object files
obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	@echo "Running tests..."
	./bin/test_serial
	./bin/test_omp
	./bin/test_strassen
	mpirun -np 4 ./bin/test_mpi
	mpirun -np 4 ./bin/test_strassen_mpi
	@echo "All tests ran!"
clean:
	rm -rf obj bin
