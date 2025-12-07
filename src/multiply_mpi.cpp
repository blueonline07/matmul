#include "matrix.h"
#include <mpi.h>

vector<double> multiply_mpi(vector<double> &A, vector<double> &B, int m, int n, int p, int rank, int size){
    int m_padded = ((m + size - 1) / size) * size;
    B.resize(n * p);
    int rows_per_proc = m_padded / size;
    vector<double> C;
    if(rank == 0){
        A.resize(m_padded * n);
        C.resize(m_padded * p);
    }
    MPI_Bcast(B.data(), n * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    vector<double> local_A(rows_per_proc * n);
    MPI_Scatter(A.data(), rows_per_proc * n, MPI_DOUBLE, local_A.data(), rows_per_proc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<double> local_C = multiply(local_A, B, rows_per_proc, n, p);

    MPI_Gather(local_C.data(), rows_per_proc * p, MPI_DOUBLE, C.data(), rows_per_proc * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(rank == 0){
        C.resize(m * p);
    }
    return C;
}