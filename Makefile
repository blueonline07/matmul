CXX = g++-15
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -fopenmp

# Define source and object files
SRCS = $(wildcard src/*.cpp)
OBJS = $(patsubst src/%.cpp,obj/%.o,$(SRCS))

# Define test source files
TEST_SRCS = $(wildcard tests/*.cpp)

# Main target
all:

# Rule to create object files
obj/%.o: src/%.cpp
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile and run the strassen test
test_strassen: $(OBJS) tests/test_strassen.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/test_strassen tests/test_strassen.cpp $(OBJS)
	./bin/test_strassen

# Rule to compile and run the serial test
test_serial: $(OBJS) tests/test_serial.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/test_serial tests/test_serial.cpp $(OBJS)
	./bin/test_serial

# Rule to compile and run the omp test
test_omp: $(OBJS) tests/test_omp.cpp
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) -o bin/test_omp tests/test_omp.cpp $(OBJS)
	./bin/test_omp

# Phony target to run all tests
.PHONY: test
test: test_strassen test_serial test_omp

# Clean rule
clean:
	rm -rf obj bin