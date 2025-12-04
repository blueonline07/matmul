#ifndef TEST_CASES_H
#define TEST_CASES_H

#include "matrix.h"
#include <cmath>
#include <functional>
#include <string>
#include <vector>
#include <iostream>

using MultiplyFunc = std::function<Matrix(const Matrix &, const Matrix &)>;

struct Implementation
{
    std::string name;
    MultiplyFunc func;
    double tolerance;
};

using TestResultFunc = std::function<void(const std::string &, bool)>;

inline void test_2x2_multiplication(const Implementation &impl, const TestResultFunc& test_result)
{
    Matrix A(2, 2);
    A(0, 0) = 1; A(0, 1) = 2;
    A(1, 0) = 3; A(1, 1) = 4;

    Matrix B(2, 2);
    B(0, 0) = 5; B(0, 1) = 6;
    B(1, 0) = 7; B(1, 1) = 8;

    Matrix C = impl.func(A, B);

    bool passed = (abs(C(0, 0) - 19) < impl.tolerance &&
                   abs(C(0, 1) - 22) < impl.tolerance &&
                   abs(C(1, 0) - 43) < impl.tolerance &&
                   abs(C(1, 1) - 50) < impl.tolerance);
    test_result(impl.name + ": 2x2 multiplication", passed);
}

inline void test_identity_multiplication(const Implementation &impl, const TestResultFunc& test_result)
{
    Matrix A(10, 10);
    A.initRandom(42);
    Matrix I(10, 10);
    I.initIdentity();
    Matrix C = impl.func(A, I);
    test_result(impl.name + ": A * I = A", C.equals(A, impl.tolerance));
}

inline void test_zero_multiplication(const Implementation &impl, const TestResultFunc& test_result)
{
    Matrix A(10, 10);
    A.initRandom(42);
    Matrix Z(10, 10);
    Z.initZeros();
    Matrix C = impl.func(A, Z);
    test_result(impl.name + ": A * 0 = 0", C.equals(Z, impl.tolerance));
}

inline void test_non_square_multiplication(const Implementation &impl, const TestResultFunc& test_result)
{
    Matrix A(2, 3);
    A(0, 0) = 1; A(0, 1) = 2; A(0, 2) = 3;
    A(1, 0) = 4; A(1, 1) = 5; A(1, 2) = 6;

    Matrix B(3, 2);
    B(0, 0) = 7; B(0, 1) = 8;
    B(1, 0) = 9; B(1, 1) = 10;
    B(2, 0) = 11; B(2, 1) = 12;

    Matrix C = impl.func(A, B);

    bool passed = (C.rows() == 2 && C.cols() == 2 &&
                   abs(C(0, 0) - 58) < impl.tolerance &&
                   abs(C(0, 1) - 64) < impl.tolerance &&
                   abs(C(1, 0) - 139) < impl.tolerance &&
                   abs(C(1, 1) - 154) < impl.tolerance);
    test_result(impl.name + ": Non-square (2x3 * 3x2)", passed);
}

inline void test_associativity(const Implementation &impl, const TestResultFunc& test_result)
{
    Matrix A(5, 5);
    Matrix B(5, 5);
    Matrix C(5, 5);
    A.initRandom(42);
    B.initRandom(43);
    C.initRandom(44);

    Matrix AB = impl.func(A, B);
    Matrix ABC1 = impl.func(AB, C);

    Matrix BC = impl.func(B, C);
    Matrix ABC2 = impl.func(A, BC);

    test_result(impl.name + ": Associativity (A*B)*C = A*(B*C)",
                ABC1.equals(ABC2, impl.tolerance * 10));
}

inline void test_correctness_vs_naive(const Implementation &impl, const TestResultFunc& test_result)
{
    if (impl.name == "Naive") return;

    Matrix A(20, 20);
    Matrix B(20, 20);
    A.initRandom(42);
    B.initRandom(43);

    Matrix C_naive = Matrix::multiplyNaive(A, B);
    Matrix C_impl = impl.func(A, B);

    test_result(impl.name + ": Correctness vs Naive (20x20)",
                C_naive.equals(C_impl, impl.tolerance));
}

inline void test_all_implementations_consistent(
    const std::vector<Implementation>& implementations,
    const TestResultFunc& test_result,
    int rank = 0)
{
    if (rank == 0) {
        std::cout << "\n--- Cross-Implementation Consistency ---" << std::endl;
    }

    Matrix A(32, 32);
    Matrix B(32, 32);
    A.initRandom(100);
    B.initRandom(101);

    Matrix C_reference = Matrix::multiplyNaive(A, B);

    bool all_consistent = true;
    for (const auto &impl : implementations)
    {
        if (impl.name == "Naive") continue;

        try
        {
            Matrix C_impl = impl.func(A, B);
            bool matches = C_reference.equals(C_impl, impl.tolerance);

            if (!matches)
            {
                all_consistent = false;
                if (rank == 0) {
                    std::cout << "[FAIL] " << impl.name << " does not match Naive" << std::endl;
                }
            }
        }
        catch (const std::exception &e)
        {
            all_consistent = false;
            if (rank == 0) {
                std::cout << "[ERROR] " << impl.name << " threw exception: " << e.what() << std::endl;
            }
        }
    }

    test_result("All implementations produce consistent results", all_consistent);
}

#endif // TEST_CASES_H