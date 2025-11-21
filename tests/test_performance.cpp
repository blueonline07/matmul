#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <mpi.h>
#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;
using namespace std::chrono;

/**
 * Performance benchmark suite for matrix multiplication implementations
 * 
 * Measures:
 * - Execution time (milliseconds)
 * - GFLOPS (Giga Floating-Point Operations Per Second)
 * - Speedup relative to naive implementation
 * 
 * Test methodology:
 * 1. Warmup run to load code into cache
 * 2. Timed run for measurement
 * 3. Multiple sizes to show scaling behavior
 */

struct BenchmarkResult
{
    string implementation;
    int size;
    double time_ms;
    double gflops;
    double speedup;
};

vector<BenchmarkResult> results;

/**
 * Benchmark a single implementation for given matrix size
 * 
 * @param name Implementation name for display
 * @param func Function pointer to multiplication implementation
 * @param size Matrix dimension (size × size)
 * @param baseline_time Time of naive implementation for speedup calculation
 */
void benchmark_implementation(const string &name,
                              Matrix (*func)(const Matrix &, const Matrix &),
                              int size,
                              double baseline_time = 0.0)
{
    cout << "    Testing " << setw(15) << left << (name + "...") << flush;
    
    Matrix A(size, size);
    Matrix B(size, size);
    A.initRandom(42);
    B.initRandom(43);

    // Warmup run to load code and data into cache
    Matrix C = func(A, B);

    // Actual benchmark - single timed run
    auto start = high_resolution_clock::now();
    C = func(A, B);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    double time_seconds = duration.count() / 1e6;
    double time_ms = time_seconds * 1000.0;
    double gflops = Matrix::gflops(size, size, size, time_seconds);
    double speedup = (baseline_time > 0.0) ? baseline_time / time_ms : 1.0;

    cout << setw(12) << right << fixed << setprecision(2) << time_ms << " ms"
         << setw(10) << right << fixed << setprecision(2) << gflops << " GFLOPS";
    
    if (baseline_time > 0.0)
    {
        cout << setw(10) << right << fixed << setprecision(2) << speedup << "x";
    }
    cout << endl;

    BenchmarkResult result;
    result.implementation = name;
    result.size = size;
    result.time_ms = time_ms;
    result.gflops = gflops;
    result.speedup = speedup;
    results.push_back(result);
}

/**
 * Print summary table of all benchmark results
 */
void print_summary()
{
    if (results.empty())
    {
        return;
    }

    cout << "\n"
         << string(90, '=') << endl;
    cout << "PERFORMANCE SUMMARY" << endl;
    cout << string(90, '=') << endl;
    cout << left << setw(20) << "Implementation"
         << right << setw(12) << "Size"
         << right << setw(15) << "Time (ms)"
         << right << setw(15) << "GFLOPS"
         << right << setw(15) << "Speedup"
         << right << setw(13) << "Efficiency" << endl;
    cout << string(90, '-') << endl;

    for (const auto &r : results)
    {
        // Calculate parallel efficiency (speedup / theoretical_max)
        // For now, just show speedup. Real efficiency needs thread/process count.
        string efficiency = (r.speedup > 1.0) 
            ? (to_string(static_cast<int>(r.speedup / r.speedup * 100)) + "%")
            : "-";

        cout << left << setw(20) << r.implementation
             << right << setw(12) << (to_string(r.size) + "×" + to_string(r.size))
             << right << setw(15) << fixed << setprecision(2) << r.time_ms
             << right << setw(15) << fixed << setprecision(3) << r.gflops
             << right << setw(15) << fixed << setprecision(2) << r.speedup << "x"
             << right << setw(13) << efficiency << endl;
    }
    cout << string(90, '=') << endl;
}

int main(int argc, char* argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    
    cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║         Matrix Multiplication - Performance Benchmark Suite          ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════════╝" << endl;
    
    // Parse command-line arguments for supercomputer use
    // Usage: ./bin/performance [size1] [size2] ... [sizeN]
    // Example: ./bin/performance 1024 2048 4096
    vector<int> sizes;
    
    if (argc > 1)
    {
        // Custom sizes provided via command line
        for (int i = 1; i < argc; i++)
        {
            int size = atoi(argv[i]);
            if (size > 0)
            {
                sizes.push_back(size);
            }
            else
            {
                cerr << "Warning: Invalid size '" << argv[i] << "', skipping" << endl;
            }
        }
        
        if (sizes.empty())
        {
            cerr << "Error: No valid sizes provided. Using defaults." << endl;
            sizes = {100, 200, 500, 1000, 2000};
        }
    }
    else
    {
        // Default sizes for development testing
        // For production benchmarks, you may want to go up to 2048 or 4096
        // Note: 10000×10000 requires ~800MB per matrix and takes extremely long
        sizes = {100, 200, 500, 1000, 2000};
    }

    cout << "\nTest configuration:" << endl;
    cout << "  Matrix sizes: ";
    for (size_t i = 0; i < sizes.size(); i++)
    {
        cout << sizes[i] << "×" << sizes[i];
        if (i < sizes.size() - 1)
            cout << ", ";
    }
    cout << endl;
    
    // Detect OpenMP threads
    int omp_threads = 1;
    #ifdef _OPENMP
    omp_threads = omp_get_max_threads();
    #endif
    
    // Check if running with MPI
    int mpi_size = 1;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    
    if (mpi_size > 1)
    {
        cout << "  MPI: " << mpi_size << " processes" << endl;
    }
    if (omp_threads > 1)
    {
        cout << "  OpenMP: " << omp_threads << " threads" << endl;
    }
    if (mpi_size > 1 && omp_threads > 1)
    {
        cout << "  Total parallelism: " << (mpi_size * omp_threads) << " (Hybrid mode)" << endl;
    }
    
    cout << "  Implementations: Naive, Strassen, OpenMP, StrassenOpenMP" << endl;
    if (mpi_size > 1)
    {
        cout << "  Note: Running with MPI - MPI variants will be tested" << endl;
    }
    cout << "\n" << string(74, '-') << endl;

    // Run benchmarks for each size
    for (int size : sizes)
    {
        cout << "\n[" << size << "×" << size << "] Benchmarking..." << endl;

        // Benchmark naive first to get baseline time
        double baseline_time = 0.0;
        
        // Naive implementation (baseline)
        benchmark_implementation("Naive", Matrix::multiplyNaive, size);
        if (!results.empty())
        {
            baseline_time = results.back().time_ms;
        }

        // Strassen algorithm
        benchmark_implementation("Strassen", Matrix::multiplyStrassen, size, baseline_time);

        // OpenMP parallel
        benchmark_implementation("OpenMP", Matrix::multiplyOpenMP, size, baseline_time);

        // Strassen with OpenMP (placeholder - falls back to sequential Strassen)
        benchmark_implementation("StrassenOpenMP", Matrix::multiplyStrassenOpenMP, size, baseline_time);

        // MPI and Hybrid variants - test if running with MPI
        // Check if compiled with MPI and running with multiple processes
        if (mpi_size > 1)
        {
            benchmark_implementation("MPI", Matrix::multiplyMPI, size, baseline_time);
            benchmark_implementation("StrassenMPI", Matrix::multiplyStrassenMPI, size, baseline_time);
            if (omp_threads > 1)
            {
                benchmark_implementation("Hybrid", Matrix::multiplyHybrid, size, baseline_time);
                benchmark_implementation("StrassenHybrid", Matrix::multiplyStrassenHybrid, size, baseline_time);
            }
        }
    }

    // Print comprehensive summary
    print_summary();

    cout << "\nBenchmark complete!" << endl;
    cout << "\nNotes:" << endl;
    cout << "  - GFLOPS = (2×n³) / (time in seconds × 10⁹)" << endl;
    cout << "  - Speedup is relative to optimized naive implementation" << endl;
    cout << "  - Results may vary based on system load and thermal throttling" << endl;
    cout << "  - For MPI tests, run with: mpirun -np <processes> ./bin/performance" << endl;

    // Finalize MPI
    MPI_Finalize();
    
    return 0;
}
