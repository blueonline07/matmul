#include "matrix.h"
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <string>
#include <mpi.h>

using namespace std;

/**
 * MPI/Hybrid Correctness Test Suite
 * 
 * Tests only MPI and Hybrid implementations:
 * - MPI (distributed memory parallelism)
 * - StrassenMPI
 * - Hybrid (MPI + OpenMP)
 * - StrassenHybrid (MPI + OpenMP with Strassen)
 */

// Test result tracking
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

// Helper function to check if matrices are approximately equal
bool matrices_equal(const Matrix &A, const Matrix &B, double tolerance = 1e-9)
{
    if (A.rows() != B.rows() || A.cols() != B.cols())
        return false;

    for (int i = 0; i < A.rows(); i++)
    {
        for (int j = 0; j < A.cols(); j++)
        {
            if (abs(A(i, j) - B(i, j)) > tolerance)
            {
                return false;
            }
        }
    }
    return true;
}

// Function pointer type for multiplication implementations
using MultiplyFunc = function<Matrix(const Matrix &, const Matrix &)>;

// Struct to hold implementation info
struct Implementation
{
    string name;
    MultiplyFunc func;
    double tolerance;
};

// List of MPI/Hybrid implementations to test
vector<Implementation> implementations = {
    {"MPI", Matrix::multiplyMPI, 1e-9},
    {"StrassenMPI", Matrix::multiplyStrassenMPI, 1e-9},
    {"Hybrid", Matrix::multiplyHybrid, 1e-9},
    {"StrassenHybrid", Matrix::multiplyStrassenHybrid, 1e-9}};

// ============================================================================
// Common Test Cases
// ============================================================================

void test_2x2_multiplication(const Implementation &impl)
{
    Matrix A(2, 2);
    A(0, 0) = 1;
    A(0, 1) = 2;
    A(1, 0) = 3;
    A(1, 1) = 4;

    Matrix B(2, 2);
    B(0, 0) = 5;
    B(0, 1) = 6;
    B(1, 0) = 7;
    B(1, 1) = 8;

    Matrix C = impl.func(A, B);

    bool passed = (abs(C(0, 0) - 19) < impl.tolerance &&
                   abs(C(0, 1) - 22) < impl.tolerance &&
                   abs(C(1, 0) - 43) < impl.tolerance &&
                   abs(C(1, 1) - 50) < impl.tolerance);
    test_result(impl.name + ": 2x2 multiplication", passed);
}

void test_identity_multiplication(const Implementation &impl)
{
    Matrix A(10, 10);
    A.initRandom(42);

    Matrix I(10, 10);
    I.initIdentity();

    Matrix C = impl.func(A, I);
    test_result(impl.name + ": A * I = A", matrices_equal(C, A, impl.tolerance));
}

void test_zero_multiplication(const Implementation &impl)
{
    Matrix A(10, 10);
    A.initRandom(42);

    Matrix Z(10, 10);
    Z.initZeros();

    Matrix C = impl.func(A, Z);
    test_result(impl.name + ": A * 0 = 0", matrices_equal(C, Z, impl.tolerance));
}

void test_non_square_multiplication(const Implementation &impl)
{
    Matrix A(2, 3);
    A(0, 0) = 1;
    A(0, 1) = 2;
    A(0, 2) = 3;
    A(1, 0) = 4;
    A(1, 1) = 5;
    A(1, 2) = 6;

    Matrix B(3, 2);
    B(0, 0) = 7;
    B(0, 1) = 8;
    B(1, 0) = 9;
    B(1, 1) = 10;
    B(2, 0) = 11;
    B(2, 1) = 12;

    Matrix C = impl.func(A, B);

    bool passed = (C.rows() == 2 && C.cols() == 2 &&
                   abs(C(0, 0) - 58) < impl.tolerance &&
                   abs(C(0, 1) - 64) < impl.tolerance &&
                   abs(C(1, 0) - 139) < impl.tolerance &&
                   abs(C(1, 1) - 154) < impl.tolerance);
    test_result(impl.name + ": Non-square (2x3 * 3x2)", passed);
}

void test_associativity(const Implementation &impl)
{
    Matrix A(5, 5);
    Matrix B(5, 5);
    Matrix C(5, 5);
    A.initRandom(42);
    B.initRandom(43);
    C.initRandom(44);

    // (A * B) * C
    Matrix AB = impl.func(A, B);
    Matrix ABC1 = impl.func(AB, C);

    // A * (B * C)
    Matrix BC = impl.func(B, C);
    Matrix ABC2 = impl.func(A, BC);

    test_result(impl.name + ": Associativity (A*B)*C = A*(B*C)",
                matrices_equal(ABC1, ABC2, impl.tolerance * 10));
}

void test_correctness_vs_naive(const Implementation &impl)
{
    Matrix A(20, 20);
    Matrix B(20, 20);
    A.initRandom(42);
    B.initRandom(43);

    Matrix C_naive = Matrix::multiplyNaive(A, B);
    Matrix C_impl = impl.func(A, B);

    test_result(impl.name + ": Correctness vs Naive (20x20)",
                matrices_equal(C_naive, C_impl, impl.tolerance));
}

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
        test_2x2_multiplication(impl);
        test_identity_multiplication(impl);
        test_zero_multiplication(impl);
        test_non_square_multiplication(impl);
        test_associativity(impl);
        test_correctness_vs_naive(impl);
    }
    catch (const exception &e)
    {
        if (mpi_rank == 0) {
            cout << "[ERROR] " << impl.name << " threw exception: " << e.what() << endl;
        }
    }
}

// ============================================================================
// Cross-implementation Consistency Tests
// ============================================================================

void test_all_implementations_consistent()
{
    if (mpi_rank == 0) {
        cout << "\n--- Cross-Implementation Consistency ---" << endl;
    }

    Matrix A(32, 32);
    Matrix B(32, 32);
    A.initRandom(100);
    B.initRandom(101);

    // Use naive as reference
    Matrix C_reference = Matrix::multiplyNaive(A, B);

    bool all_consistent = true;
    for (const auto &impl : implementations)
    {
        try
        {
            Matrix C_impl = impl.func(A, B);
            bool matches = matrices_equal(C_reference, C_impl, impl.tolerance);

            if (!matches)
            {
                all_consistent = false;
                if (mpi_rank == 0) {
                    cout << "[FAIL] " << impl.name << " does not match Naive" << endl;
                }
            }
        }
        catch (const exception &e)
        {
            all_consistent = false;
            if (mpi_rank == 0) {
                cout << "[ERROR] " << impl.name << " threw exception: " << e.what() << endl;
            }
        }
    }

    test_result("All implementations produce consistent results", all_consistent);
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main(int argc, char** argv)
{
    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    
    int mpi_size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    
    if (mpi_rank == 0) {
        cout << endl;
        cout << "╔════════════════════════════════════════════════════════════╗" << endl;
        cout << "║        Matrix Multiplication - TDD Test Suite              ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════╝\n" << endl;
    }

    // Run the test suite for each MPI/Hybrid implementation
    for (const auto &impl : implementations)
    {
        run_test_suite(impl);
    }

    // Cross-implementation consistency check
    test_all_implementations_consistent();

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

    // Finalize MPI
    MPI_Finalize();
    
    return (passed_tests == total_tests) ? 0 : 1;
}

