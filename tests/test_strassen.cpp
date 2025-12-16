#include "matrix.h"
#include "test_cases.h"
#include <mpi.h>
#include <cassert>

/*
    test distribute strassen, due to it nature of needing exactly 7 proc
*/

void test_strassen_hybrid(int N, int rank, int size)
{
    int m = N, n = N, p = N;
    vector<double> A;
    vector<double> B;
    if (rank == 0)
    {
        A.resize(m * n);
        for (int i = 0; i < m * n; i++)
        {
            A[i] = 1;
        }
        B.resize(n * p);
        for (int i = 0; i < n * p; i++)
        {
            B[i] = 1;
        }
    }

    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = strassen_hybrid(A, B, m, n, p, rank, size);
    auto t1 = chrono::high_resolution_clock::now();

    if (rank == 0)
    {
        cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
        t0 = chrono::high_resolution_clock::now();
        assert(C == libcheck(A, B, m, n, p));
        cout << "eigen: " << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
    }
}

void test_strassen_mpi(int N, int rank, int size)
{
    int m = N, n = N, p = N;
    vector<double> A;
    vector<double> B;
    if (rank == 0)
    {
        A.resize(m * n);
        for (int i = 0; i < m * n; i++)
        {
            A[i] = 1;
        }
        B.resize(n * p);
        for (int i = 0; i < n * p; i++)
        {
            B[i] = 1;
        }
    }

    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = strassen_mpi(A, B, m, n, p, rank, size);
    auto t1 = chrono::high_resolution_clock::now();

    if (rank == 0)
    {
        cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
        t0 = chrono::high_resolution_clock::now();
        assert(C == libcheck(A, B, m, n, p));
        cout << "eigen: " << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int N = 1000;
    if (argc > 1)
    {
        N = atoi(argv[1]);
    }
    test_strassen_mpi(N, rank, size);
    test_strassen_hybrid(N, rank, size);
    MPI_Finalize();
    return 0;
}
