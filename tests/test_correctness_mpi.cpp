#include "matrix.h"
#include "test_cases.h"
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <string>
#include <mpi.h>

using namespace std;

int total_tests = 0;
int passed_tests = 0;
int mpi_rank = 0;

void test_result(const string &test_name, bool passed)
{
    total_tests++;
    if (passed)
    {
        passed_tests++;
        if (mpi_rank == 0) {
            cout << "[PASS] " << test_name << endl;
        }
    }
    else
    {
        if (mpi_rank == 0) {
            cout << "[FAIL] " << test_name << endl;
        }
    }
}

vector<Implementation> implementations;

// ============================================================================
// Run all test cases for each implementation
// ============================================================================

void run_test_suite(const Implementation &impl)
{
    if (mpi_rank == 0) {
        cout << "\n--- Testing: " << impl.name << " ---" << endl;
    }

    try
    {
        test_2x2_multiplication(impl, test_result);
        test_identity_multiplication(impl, test_result);
        test_zero_multiplication(impl, test_result);
        test_non_square_multiplication(impl, test_result);
        test_associativity(impl, test_result);
        test_correctness_vs_naive(impl, test_result);
    }
    catch (const exception &e)
    {
        if (mpi_rank == 0) {
            cout << "[ERROR] " << impl.name << " threw exception: " << e.what() << endl;
        }
    }
}

int main(int argc, char** argv)
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    
    int mpi_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    implementations = {
        {"MPI", 
            [&](const Matrix& a, const Matrix& b){ return Matrix::multiplyMPI(a, b, mpi_size, mpi_rank); }, 
            1e-9},
        {"StrassenMPI", 
            [&](const Matrix& a, const Matrix& b){ return Matrix::multiplyStrassenMPI(a, b, mpi_size, mpi_rank); }, 
            1e-9},
        {"Hybrid", 
            [&](const Matrix& a, const Matrix& b){ return Matrix::multiplyHybrid(a, b, mpi_size, mpi_rank); }, 
            1e-9},
        {"StrassenHybrid", 
            [&](const Matrix& a, const Matrix& b){ return Matrix::multiplyStrassenHybrid(a, b, mpi_size, mpi_rank); }, 
            1e-9}
    };
    
    if (mpi_rank == 0) {
        cout << endl;
        cout << "╔════════════════════════════════════════════════════════════╗" << endl;
        cout << "║        Matrix Multiplication - TDD Test Suite              ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════╝\n" << endl;
    }

    for (const auto &impl : implementations)
    {
        run_test_suite(impl);
    }

    test_all_implementations_consistent(implementations, test_result, mpi_rank);

    if (mpi_rank == 0) {
        cout << "\n" << string(60, '=') << endl;
        cout << "RESULTS: " << passed_tests << "/" << total_tests << " tests passed" << endl;
        cout << string(60, '=') << endl;

        if (passed_tests == total_tests)
        {
            cout << "\n✓ All tests PASSED!" << endl;
        }
        else
        {
            cout << "\n✗ " << (total_tests - passed_tests) << " test(s) FAILED" << endl;
        }
    }

    MPI_Finalize();
    
    return (passed_tests == total_tests) ? 0 : 1;
}

