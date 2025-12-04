#include "matrix.h"
#include "test_cases.h"

int main() {
    test_simple_serial();
    test_serial_zeros();
    test_serial_identity();
    test_serial_large();
    return 0;
}

void test_simple_serial(){
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };

    vector<double> C = {
        1, 2, 3, 2, 4, 6
    };
    
    assert(multiply(A, B, m, n, p) == C);
}

void test_serial_zeros() {
    int m = 3, n = 2, p = 4;
    vector<double> A = {1, 2, 3, 4, 5, 6};
    vector<double> B(n * p, 0);
    vector<double> C(m * p, 0);

    assert(multiply(A, B, m, n, p) == C);
}

void test_serial_identity() {
    int m = 3, n = 3, p = 3;
    vector<double> A = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    vector<double> B = {
        1, 0, 0,
        0, 1, 0,
        0, 0, 1
    };

    assert(multiply(A, B, m, n, p) == A);
}

void test_serial_large() {
    int m = 1000, n = 1000, p = 1000;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++) {
        A[i] = i;
    }

    for (int i = 0; i < n * p; i++) {
        B[i] = i;
    }
    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = multiply(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();
    cout<<chrono::duration_cast<chrono::duration<double>> (t1 - t0).count()<<endl;

    vector<double> C_expected(m * p, 0);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < p; j++) {
            for (int k = 0; k < n; k++) {
                C_expected[i * p + j] += A[i * n + k] * B[k * p + j];
            }
        }
    }

    assert(C == C_expected);
}