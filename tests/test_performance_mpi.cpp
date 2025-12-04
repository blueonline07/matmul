#include "matrix.h"
#include "benchmark_cases.h"
#include <mpi.h>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

int main(int argc, char* argv[])
{
    int mpi_rank = 0;
    int mpi_size = 1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    
    if (mpi_rank == 0) {
        cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║         Matrix Multiplication - Performance Benchmark Suite          ║" << endl;
        cout << "╚══════════════════════════════════════════════════════════════════════╝" << endl;
    }
    
    vector<int> sizes;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            int size = atoi(argv[i]);
            if (size > 0) sizes.push_back(size);
            else if (mpi_rank == 0) cerr << "Warning: Invalid size '" << argv[i] << "', skipping" << endl;
        }
        if (sizes.empty())
        {
            if (mpi_rank == 0) cerr << "Error: No valid sizes provided. Using defaults." << endl;
            sizes = {100, 200, 500, 1000};
        }
    }
    else
    {
        sizes = {100, 200, 500, 1000};
    }

    int omp_threads = 1;
    #ifdef _OPENMP
    #pragma omp parallel
    {
        #pragma omp single
        omp_threads = omp_get_num_threads();
    }
    #endif

    if (mpi_rank == 0) {
        cout << "\nTest configuration:" << endl;
        cout << "  MPI Processes: " << mpi_size << endl;
        cout << "  Matrix sizes: ";
        for (size_t i = 0; i < sizes.size(); i++)
        {
            cout << sizes[i] << "×" << sizes[i] << (i < sizes.size() - 1 ? ", " : "");
        }
        cout << endl;
        
        if (omp_threads > 1)
        {
            cout << "  OpenMP: " << omp_threads << " threads per process" << endl;
        }
        
        cout << "  Implementations: MPI, StrassenMPI, Hybrid, StrassenHybrid" << endl;
        cout << "\n" << string(74, '-') << endl;
    }

    vector<BenchmarkResult> results;
    for (int size : sizes)
    {
        if (mpi_rank == 0) {
            cout << "\n[" << size << "×" << size << "] Benchmarking..." << endl;
        }

        double baseline_time = 0.0;
        if (mpi_rank == 0) {
            Matrix A(size, size), B(size, size);
            A.initRandom(42); B.initRandom(43);
            auto start = high_resolution_clock::now();
            Matrix C = Matrix::multiplyNaive(A, B);
            auto end = high_resolution_clock::now();
            baseline_time = duration_cast<microseconds>(end - start).count() / 1000.0;
        }
        
        MPI_Bcast(&baseline_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        auto mpi_func = [&](const Matrix& a, const Matrix& b) { return Matrix::multiplyMPI(a, b, mpi_size, mpi_rank); };
        benchmark_implementation("MPI", mpi_func, size, mpi_size, baseline_time, results, mpi_rank);

        auto strassen_mpi_func = [&](const Matrix& a, const Matrix& b) { return Matrix::multiplyStrassenMPI(a, b, mpi_size, mpi_rank); };
        benchmark_implementation("StrassenMPI", strassen_mpi_func, size, mpi_size, baseline_time, results, mpi_rank);

        auto hybrid_func = [&](const Matrix& a, const Matrix& b) { return Matrix::multiplyHybrid(a, b, mpi_size, mpi_rank); };
        benchmark_implementation("Hybrid", hybrid_func, size, mpi_size * omp_threads, baseline_time, results, mpi_rank);

        auto strassen_hybrid_func = [&](const Matrix& a, const Matrix& b) { return Matrix::multiplyStrassenHybrid(a, b, mpi_size, mpi_rank); };
        benchmark_implementation("StrassenHybrid", strassen_hybrid_func, size, mpi_size * omp_threads, baseline_time, results, mpi_rank);
    }

    print_summary(results, mpi_rank);
    MPI_Finalize();
    return 0;
}

