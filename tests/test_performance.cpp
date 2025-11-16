#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>

using namespace std;
using namespace std::chrono;

struct BenchmarkResult
{
    string implementation;
    int size;
    double time_ms;
    double gflops;
};

vector<BenchmarkResult> results;

void benchmark_implementation(const string &name,
                              Matrix (*func)(const Matrix &, const Matrix &),
                              int size)
{
    Matrix A(size, size);
    Matrix B(size, size);
    A.initRandom(42);
    B.initRandom(43);

    // Warmup
    Matrix C = func(A, B);

    // Actual benchmark
    auto start = high_resolution_clock::now();
    C = func(A, B);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    double time_seconds = duration.count() / 1e6;
    double gflops = Matrix::gflops(size, size, size, time_seconds);

    BenchmarkResult result;
    result.implementation = name;
    result.size = size;
    result.time_ms = time_seconds * 1000;
    result.gflops = gflops;
    results.push_back(result);
}

void print_results()
{
    cout << "\n"
         << string(70, '=') << endl;
    cout << left << setw(20) << "Implementation"
         << setw(10) << "Size"
         << setw(15) << "Time (ms)"
         << setw(15) << "GFLOPS"
         << setw(10) << "Speedup" << endl;
    cout << string(70, '-') << endl;

    double baseline_time = 0;
    for (const auto &r : results)
    {
        if (r.implementation == "Naive")
        {
            baseline_time = r.time_ms;
        }
    }

    for (const auto &r : results)
    {
        double speedup = (baseline_time > 0 && r.implementation != "Naive")
                             ? baseline_time / r.time_ms
                             : 1.0;

        cout << left << setw(20) << r.implementation
             << setw(10) << (to_string(r.size) + "x" + to_string(r.size))
             << setw(15) << fixed << setprecision(2) << r.time_ms
             << setw(15) << fixed << setprecision(2) << r.gflops
             << setw(10) << fixed << setprecision(2) << speedup << "x" << endl;
    }
    cout << string(70, '=') << endl;
}

int main()
{
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║           Matrix Multiplication - Performance Tests        ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝\n"
         << endl;

    vector<int> sizes = {100, 200, 500, 1000};

    for (int size : sizes)
    {
        cout << "\nBenchmarking size: " << size << "x" << size << endl;

        cout << "  Testing Naive..." << endl;
        benchmark_implementation("Naive", Matrix::multiplyNaive, size);

        cout << "  Testing Strassen..." << endl;
        benchmark_implementation("Strassen", Matrix::multiplyStrassen, size);

        cout << "  Testing OpenMP..." << endl;
        benchmark_implementation("OpenMP", Matrix::multiplyOpenMP, size);

        // cout << "  Testing MPI..." << endl;
        // benchmark_implementation("MPI", Matrix::multiplyMPI, size);

        // cout << "  Testing Hybrid..." << endl;
        // benchmark_implementation("Hybrid", Matrix::multiplyHybrid, size);

        print_results();
        results.clear();
    }

    return 0;
}
