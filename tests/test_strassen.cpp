#include "matrix.h"
#include "test_cases.h"
#include <cassert>

void test_simple_strassen() {
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };

    vector<double> C = {
        1, 2, 3, 2, 4, 6
    };

    assert(strassen(A, B, m, n, p) == C);
}

void test_strassen_large(int N) {
    int m = N, n = N, p = N;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++) {
        A[i] = i;
    }

    for (int i = 0; i < n * p; i++) {
        B[i] = i;
    }
    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = strassen(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();
    cout<<chrono::duration_cast<chrono::duration<double>> (t1 - t0).count()<<endl;

    vector<double> C_expected = multiply(A, B, m, n, p);

    assert(C == C_expected);
}

int main() {
    test_simple_strassen();
    test_strassen_large(1000);
    return 0;
}
