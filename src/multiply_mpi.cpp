#include "matrix.h"
#include <mpi.h>

vector<double> multiply_mpi(const vector<double> &A, vector<double> &B, int m, int n, int p, int rank, int size){
    int m_padded = ((m + size - 1) / size) * size;
    vector<double> A_padded(m_padded * n);
    vector<double> C_padded(m_padded * p);
    vector<double> C(m * p);
    int rows_per_proc = m_padded / size;

    if(rank == 0){
        for(int i = 0; i < m; i++){
            for(int j = 0; j < n; j++){
                A_padded[i * n + j] = A[i * n + j];
            }
        }
    }
    MPI_Bcast(B.data(), n * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    vector<double> local_A(rows_per_proc * n);
    MPI_Scatter(A_padded.data(), rows_per_proc * n, MPI_DOUBLE, local_A.data(), rows_per_proc * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<double> local_C = multiply(local_A, B, rows_per_proc, n, p);

    MPI_Gather(local_C.data(), rows_per_proc * p, MPI_DOUBLE, C_padded.data(), rows_per_proc * p, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if(rank == 0){
        for(int i = 0; i < m; i++){
            for(int j = 0; j < p; j++){
                C[i * p + j] = C_padded[i * p + j];
            }
        }
    }
    return C;
}