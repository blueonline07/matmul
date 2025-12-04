#include "matrix.h"
#include "test_cases.h"
#include <iostream>
#include <cmath>
#include <functional>
#include <vector>
#include <string>

using namespace std;
int total_tests = 0;
int passed_tests = 0;

void test_result(const string &test_name, bool passed)
{
    total_tests++;
    if (passed)
    {
        passed_tests++;
        cout << "[PASS] " << test_name << endl;
    }
    else
    {
        cout << "[FAIL] " << test_name << endl;
    }
}

vector<Implementation> implementations = {
    {"Naive", Matrix::multiplyNaive, 1e-9},
    {"Strassen", Matrix::multiplyStrassen, 1e-9},
    {"OpenMP", Matrix::multiplyOpenMP, 1e-9},
    {"StrassenOpenMP", Matrix::multiplyStrassenOpenMP, 1e-9}};

// ============================================================================
// Run all test cases for each implementation
// ============================================================================

void run_test_suite(const Implementation &impl)
{
    cout << "\n--- Testing: " << impl.name << " ---" << endl;

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
        cout << "[ERROR] " << impl.name << " threw exception: " << e.what() << endl;
    }
}

int main()
{
    cout << endl;
    cout << "╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║        Matrix Multiplication - TDD Test Suite              ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝\n"
         << endl;

    for (const auto &impl : implementations)
    {
        run_test_suite(impl);
    }

    test_all_implementations_consistent(implementations, test_result);

    cout << "\n"
         << string(60, '=') << endl;
    cout << "RESULTS: " << passed_tests << "/" << total_tests << " tests passed" << endl;
    cout << string(60, '=') << endl;

    if (passed_tests == total_tests)
    {
        cout << "\n✓ All tests PASSED!" << endl;
    }
    else
    {
        cout << "\n✗ " << (total_tests - passed_tests) << " test(s) FAILED" << endl;
        cout << "Implement missing features to make tests pass." << endl;
    }

    return (passed_tests == total_tests) ? 0 : 1;
}
