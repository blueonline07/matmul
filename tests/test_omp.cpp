#include "matrix.h"
#include "test_cases.h"
#include <cassert>

void test_omp_simple() {
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };
    
    assert(multiply_omp(A, B, m, n, p) == libcheck(A, B, m, n, p));
}

void test_omp_large(int N) {
    int m = N, n = N, p = N;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++) {
        A[i] = 1;
    }

    for (int i = 0; i < n * p; i++) {
        B[i] = 1;
    }
    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = multiply_omp(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();
    cout<<chrono::duration_cast<chrono::duration<double>> (t1 - t0).count()<<endl;
    assert(C == libcheck(A, B, m, n, p));
}

int main() {
    test_omp_simple();
    test_omp_large(1000);
    return 0;
}
