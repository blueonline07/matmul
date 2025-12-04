#include "matrix.h"
#include "test_cases.h"

int main() {
    test_simple_serial();
    test_serial_zeros();
    test_serial_identity();
    test_serial_large(1000);
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


void test_serial_large(int N) {
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
    vector<double> C = multiply(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << " ";

    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        A_eig(A.data(), m, n);

    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        B_eig(B.data(), n, p);
    t0 = chrono::high_resolution_clock::now();
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
        C_eig = A_eig * B_eig;
    vector<double> C_expected(m * p);
    t1 = chrono::high_resolution_clock::now();
    cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
    memcpy(C_expected.data(), C_eig.data(), sizeof(double) * m * p);

    assert(C == C_expected);
}