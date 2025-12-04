#include "matrix.h"
#include "test_cases.h"
#include <mpi.h>
#include <cassert>

void test_simple_mpi(int rank, int size) {
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };

    vector<double> C = {
        1, 2, 3, 2, 4, 6
    };

    vector<double> result = multiply_mpi(A, B, m, n, p, rank, size);

    if(rank == 0)
        assert(result == C);
}

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    test_simple_mpi(rank, size);
    MPI_Finalize();
    return 0;
}
