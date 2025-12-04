#include "matrix.h"
#include "test_cases.h"
#include <mpi.h>
#include <cassert>

void test_strassen_mpi_simple(int rank, int size) {
    int m = 4, n = 4, p = 4;
    vector<double> A = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };

    vector<double> B = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };

    vector<double> C = {
        90, 100, 110, 120,
        202, 228, 254, 280,
        314, 356, 398, 440,
        426, 484, 542, 600
    };

    vector<double> result = strassen_mpi(A, B, m, n, p, rank, size);

    if(rank == 0)
        assert(result == C);
}

void test_strassen_mpi_zeros(int rank, int size) {
    int m = 4, n = 4, p = 4;
    vector<double> A = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    vector<double> B(n * p, 0);
    vector<double> C(m * p, 0);

    vector<double> result = strassen_mpi(A, B, m, n, p, rank, size);

    if(rank == 0)
        assert(result == C);
}

void test_strassen_mpi_identity(int rank, int size) {
    int m = 4, n = 4, p = 4;
    vector<double> A = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16
    };
    vector<double> B = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    vector<double> result = strassen_mpi(A, B, m, n, p, rank, size);

    if(rank == 0)
        assert(result == A);
}

void test_strassen_mpi_large(int rank, int size) {
    int m = 128, n = 128, p = 128;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++) {
        A[i] = i;
    }

    for (int i = 0; i < n * p; i++) {
        B[i] = i;
    }

    vector<double> C = strassen_mpi(A, B, m, n, p, rank, size);

    if (rank == 0) {
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
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    test_strassen_mpi_simple(rank, size);
    test_strassen_mpi_zeros(rank, size);
    test_strassen_mpi_identity(rank, size);
    test_strassen_mpi_large(rank, size);

    MPI_Finalize();
    return 0;
}