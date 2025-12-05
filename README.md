# Matrix Multiplication

This project provides a collection of C++ implementations for matrix multiplication, exploring different parallelization strategies and algorithms.

## Implementations

The project includes the following implementations:

-   **Serial**: A naive matrix multiplication implementation with loop blocking and SIMD optimizations.
-   **OpenMP**: A parallel version of the naive implementation using OpenMP for shared-memory parallelism.
-   **MPI**: A parallel version of the naive implementation using MPI for distributed-memory parallelism.
-   **Hybrid (MPI + OpenMP)**: A hybrid version combining MPI and OpenMP for parallelism.
-   **Strassen's Algorithm**: A serial implementation of Strassen's algorithm, a recursive method for faster matrix multiplication.
-   **Strassen's Algorithm with OpenMP**: A parallel version of Strassen's algorithm using OpenMP.
-   **Strassen's Algorithm with MPI**: A parallel version of Strassen's algorithm using MPI.
-   **Strassen's Algorithm with Hybrid (MPI + OpenMP)**: A hybrid version of Strassen's algorithm combining MPI and OpenMP.

## Prerequisites

-   A C++23 compatible compiler (e.g., g++-15 or newer)
-   OpenMP
-   An MPI implementation (e.g., Open MPI)
-   Eigen3 library (for testing)

## How to Build

To build all the test executables, run:

```bash
make all
```

This will create the following executables in the `bin/` directory:

-   `test_serial`
-   `test_omp`
-   `test_mpi`
-   `test_hybrid`

## How to Run

You can run the tests using the `make` command. The default matrix size is 1000x1000, but you can change it by passing the `N` variable.

-   **Serial:**
    ```bash
    make run_test_serial N=2000
    ```

-   **OpenMP:**
    ```bash
    make run_test_omp N=2000
    ```

-   **MPI:**
    ```bash
    make run_test_mpi N=2000
    ```
    This will run with 4 MPI processes by default.

-   **Hybrid (MPI + OpenMP):**
    ```bash
    make run_test_hybrid N=2000
    ```
    This will run with 4 MPI processes by default.

## How to Test

To run all the tests with a specific matrix size:

```bash
make test N=500
```