#ifndef BENCHMARK_CASES_H
#define BENCHMARK_CASES_H

#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <functional>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace std::chrono;

struct BenchmarkResult
{
    std::string implementation;
    int size;
    double time_ms;
    double speedup;
    int workers;
};

using BenchmarkFunc = std::function<Matrix(const Matrix&, const Matrix&)>;

inline void benchmark_implementation(
    const std::string &name,
    BenchmarkFunc func,
    int size,
    int workers,
    double baseline_time,
    std::vector<BenchmarkResult>& results,
    int rank = 0)
{
    if (rank == 0) {
        std::cout << "    Testing " << std::setw(15) << std::left << (name + "...") << std::flush;
    }
    
    Matrix A(size, size);
    Matrix B(size, size);
    A.initRandom(42);
    B.initRandom(43);

    // Warmup run
    Matrix C_warmup = func(A, B);

    // Actual benchmark
    auto start = high_resolution_clock::now();
    Matrix C = func(A, B);
    auto end = high_resolution_clock::now();

    double time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    double speedup = (baseline_time > 0.0) ? baseline_time / time_ms : 1.0;

    if (rank == 0) {
        std::cout << std::setw(12) << std::right << std::fixed << std::setprecision(2) << time_ms << " ms";
        if (baseline_time > 0.0)
        {
            std::cout << std::setw(10) << std::right << std::fixed << std::setprecision(2) << speedup << "x";
        }
        std::cout << std::endl;

        BenchmarkResult result;
        result.implementation = name;
        result.size = size;
        result.time_ms = time_ms;
        result.speedup = speedup;
        result.workers = workers;
        results.push_back(result);
    }
}

inline void print_summary(const std::vector<BenchmarkResult>& results, int rank = 0)
{
    if (rank == 0) {
        if (results.empty())
        {
            return;
        }

        std::cout << "\n"
             << std::string(90, '=') << std::endl;
        std::cout << "PERFORMANCE SUMMARY" << std::endl;
        std::cout << std::string(90, '=') << std::endl;
        std::cout << std::left << std::setw(20) << "Implementation"
             << std::right << std::setw(12) << "Size"
             << std::right << std::setw(15) << "Time (ms)"
             << std::right << std::setw(15) << "Speedup"
             << std::right << std::setw(13) << "Efficiency" << std::endl;
        std::cout << std::string(90, '-') << std::endl;

        for (const auto &r : results)
        {
            std::string efficiency = "-";
            if (r.workers > 1)
            {
                double real_efficiency = (r.speedup / r.workers) * 100.0;
                efficiency = std::to_string(static_cast<int>(real_efficiency)) + "%";
            }

            std::cout << std::left << std::setw(20) << r.implementation
                 << std::right << std::setw(12) << (std::to_string(r.size) + "×" + std::to_string(r.size))
                 << std::right << std::setw(15) << std::fixed << std::setprecision(2) << r.time_ms
                 << std::right << std::setw(15) << std::fixed << std::setprecision(2) << r.speedup << "x"
                 << std::right << std::setw(13) << efficiency << std::endl;
        }
        std::cout << std::string(90, '=') << std::endl;
    }
}

#endif // BENCHMARK_CASES_H
