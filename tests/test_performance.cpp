#include "matrix.h"
#include "benchmark_cases.h"

#include <vector>
#include <string>
#include <iostream>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std;

int main(int argc, char* argv[])
{
    cout << "\n╔══════════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║         Matrix Multiplication - Performance Benchmark Suite          ║" << endl;
    cout << "╚══════════════════════════════════════════════════════════════════════╝" << endl;
    
    vector<int> sizes;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            int size = atoi(argv[i]);
            if (size > 0) sizes.push_back(size);
            else cerr << "Warning: Invalid size '" << argv[i] << "', skipping" << endl;
        }
        if (sizes.empty())
        {
            cerr << "Error: No valid sizes provided. Using defaults." << endl;
            sizes = {100, 200, 500, 1000};
        }
    }
    else
    {
        sizes = {100, 200, 500, 1000};
    }

    cout << "\nTest configuration:" << endl;
    cout << "  Matrix sizes: ";
    for (size_t i = 0; i < sizes.size(); i++)
    {
        cout << sizes[i] << "×" << sizes[i] << (i < sizes.size() - 1 ? ", " : "");
    }
    cout << endl;
    
    int omp_threads = 1;
    #ifdef _OPENMP
    omp_threads = omp_get_max_threads();
    #endif
    
    if (omp_threads > 1)
    {
        cout << "  OpenMP: " << omp_threads << " threads" << endl;
    }
    
    cout << "  Implementations: Naive, Strassen, OpenMP, StrassenOpenMP" << endl;
    cout << "\n" << string(74, '-') << endl;

    vector<BenchmarkResult> results;
    for (int size : sizes)
    {
        cout << "\n[" << size << "×" << size << "] Benchmarking..." << endl;

        double baseline_time = 0.0;
        
        benchmark_implementation("Naive", Matrix::multiplyNaive, size, 1, 0.0, results);
        if (!results.empty())
        {
            baseline_time = results.back().time_ms;
        }

        benchmark_implementation("Strassen", Matrix::multiplyStrassen, size, 1, baseline_time, results);
        benchmark_implementation("OpenMP", Matrix::multiplyOpenMP, size, omp_threads, baseline_time, results);
        benchmark_implementation("StrassenOpenMP", Matrix::multiplyStrassenOpenMP, size, omp_threads, baseline_time, results);
    }

    print_summary(results);
    return 0;
}
