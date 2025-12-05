#include "matrix.h"
#include "test_cases.h"
#include <mpi.h>
#include <cassert>

void test_hybrid_simple(int rank, int size) {
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };

    vector<double> C = {
        1, 2, 3, 2, 4, 6
    };

    vector<double> result = multiply_hybrid(A, B, m, n, p, rank, size);

    if(rank == 0)
        assert(result == C);
}

void test_hybrid_large(int N,int rank, int size){
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
    vector<double> C = multiply_hybrid(A, B, m, n, p, rank, size);
    auto t1 = chrono::high_resolution_clock::now();
    
    if(rank == 0){
        cout<<chrono::duration_cast<chrono::duration<double>> (t1 - t0).count()<<endl;
        assert(C == libcheck(A, B, m, n, p));
    }  
}

#include <cstdlib>

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    test_hybrid_simple(rank, size);
    int N = 1000;
    if (argc > 1) {
        N = atoi(argv[1]);
    }
    test_hybrid_large(N, rank, size);
    MPI_Finalize();
    return 0;
}
