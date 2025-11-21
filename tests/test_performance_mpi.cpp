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
 * MPI/Hybrid Performance Benchmark Suite
 * 
 * Tests only MPI and Hybrid implementations:
 * - MPI (distributed memory parallelism)
 * - StrassenMPI
 * - Hybrid (MPI + OpenMP)
 * - StrassenHybrid (MPI + OpenMP with Strassen)
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
int mpi_rank = 0;

void benchmark_implementation(const string &name,
                              Matrix (*func)(const Matrix &, const Matrix &),
                              int size,
                              double baseline_time = 0.0)
{
    // Create random matrices
    Matrix A(size, size);
    Matrix B(size, size);
    A.initRandom(42);
    B.initRandom(43);

    // Warmup run
    Matrix C_warmup = func(A, B);

    // Timed run
    auto start = high_resolution_clock::now();
    Matrix C = func(A, B);
    auto end = high_resolution_clock::now();

    double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;

    // Calculate GFLOPS: (2 * n^3) / (time_in_seconds * 10^9)
    double operations = 2.0 * size * size * size;
    double gflops = operations / (time_ms * 1e6);

    // Calculate speedup relative to baseline
    double speedup = (baseline_time > 0) ? (baseline_time / time_ms) : 1.0;

    // Store result (only rank 0 stores and prints)
    if (mpi_rank == 0) {
        BenchmarkResult result;
        result.implementation = name;
        result.size = size;
        result.time_ms = time_ms;
        result.gflops = gflops;
        result.speedup = speedup;
        results.push_back(result);

        cout << "    Testing " << setw(15) << left << (name + "...") 
             << setw(12) << right << fixed << setprecision(2) << time_ms << " ms"
             << setw(10) << right << fixed << setprecision(2) << gflops << " GFLOPS";
        
        if (baseline_time > 0.0)
        {
            cout << setw(10) << right << fixed << setprecision(2) << speedup << "x";
        }
        cout << endl;
    }
}

void print_summary()
{
    if (mpi_rank == 0) {
        cout << "\n" << string(90, '=') << endl;
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
            string efficiency = (r.speedup >= 1.0)
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
}

int main(int argc, char* argv[])
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    
    int mpi_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    
    if (mpi_rank == 0) {
        cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║         Matrix Multiplication - Performance Benchmark Suite          ║" << endl;
        cout << "╚══════════════════════════════════════════════════════════════════════╝" << endl;
    }
    
    // Parse command-line arguments
    vector<int> sizes;
    
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            int size = atoi(argv[i]);
            if (size > 0)
            {
                sizes.push_back(size);
            }
            else
            {
                if (mpi_rank == 0) {
                    cerr << "Warning: Invalid size '" << argv[i] << "', skipping" << endl;
                }
            }
        }
        
        if (sizes.empty())
        {
            if (mpi_rank == 0) {
                cerr << "Error: No valid sizes provided. Using defaults." << endl;
            }
            sizes = {100, 200, 500, 1000};
        }
    }
    else
    {
        sizes = {100, 200, 500, 1000};
    }

    if (mpi_rank == 0) {
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
        
        if (omp_threads > 1)
        {
            cout << "  OpenMP: " << omp_threads << " threads" << endl;
        }
        
        cout << "  Implementations: MPI, StrassenMPI, Hybrid, StrassenHybrid" << endl;
        cout << "\n" << string(74, '-') << endl;
    }

    // Run benchmarks for each size
    for (int size : sizes)
    {
        if (mpi_rank == 0) {
            cout << "\n[" << size << "×" << size << "] Benchmarking..." << endl;
        }

        // Benchmark naive first to get baseline time (only rank 0)
        double baseline_time = 0.0;
        
        if (mpi_rank == 0) {
            Matrix A(size, size);
            Matrix B(size, size);
            A.initRandom(42);
            B.initRandom(43);
            
            // Naive implementation (baseline)
            auto start = high_resolution_clock::now();
            Matrix C = Matrix::multiplyNaive(A, B);
            auto end = high_resolution_clock::now();
            
            baseline_time = duration_cast<microseconds>(end - start).count() / 1000.0;
        }
        
        // Broadcast baseline time to all ranks for consistent speedup calculation
        MPI_Bcast(&baseline_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        // MPI implementations
        benchmark_implementation("MPI", Matrix::multiplyMPI, size, baseline_time);
        benchmark_implementation("StrassenMPI", Matrix::multiplyStrassenMPI, size, baseline_time);
        benchmark_implementation("Hybrid", Matrix::multiplyHybrid, size, baseline_time);
        benchmark_implementation("StrassenHybrid", Matrix::multiplyStrassenHybrid, size, baseline_time);
    }

    // Print comprehensive summary
    print_summary();

    if (mpi_rank == 0) {
        cout << "\nBenchmark complete!" << endl;
        cout << "\nNotes:" << endl;
        cout << "  - GFLOPS = (2×n³) / (time in seconds × 10⁹)" << endl;
        cout << "  - Speedup is relative to naive implementation" << endl;
        cout << "  - Results may vary based on system load and thermal throttling" << endl;
    }

    // Finalize MPI
    MPI_Finalize();
    
    return 0;
}

