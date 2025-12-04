#include "matrix.h"
#include <mpi.h>

vector<double> strassen_mpi(const vector<double> &A, vector<double> &B, int m, int n, int p, int rank, int size)
{
    // Base case: use standard multiplication for small matrices or non-square
    if (m <= 64 || m != n || n != p) {
        return multiply_mpi(A, B, m, n, p, rank, size);
    }
    
    int h = m / 2;
    int hs = h * h;
    
    // Only rank 0 partitions the matrices
    vector<double> A11, A12, A21, A22;
    vector<double> B11, B12, B21, B22;
    
    if (rank == 0) {
        A11.resize(hs); A12.resize(hs); A21.resize(hs); A22.resize(hs);
        B11.resize(hs); B12.resize(hs); B21.resize(hs); B22.resize(hs);
        
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < h; j++) {
                A11[i * h + j] = A[i * m + j];
                A12[i * h + j] = A[i * m + j + h];
                A21[i * h + j] = A[(i + h) * m + j];
                A22[i * h + j] = A[(i + h) * m + j + h];
                
                B11[i * h + j] = B[i * n + j];
                B12[i * h + j] = B[i * n + j + h];
                B21[i * h + j] = B[(i + h) * n + j];
                B22[i * h + j] = B[(i + h) * n + j + h];
            }
        }
    } else {
        A11.resize(hs); A12.resize(hs); A21.resize(hs); A22.resize(hs);
        B11.resize(hs); B12.resize(hs); B21.resize(hs); B22.resize(hs);
    }
    
    // Broadcast all submatrices to all processes
    MPI_Bcast(A11.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(A12.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(A21.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(A22.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B11.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B12.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B21.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(B22.data(), hs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Distribute computation of M1-M7 across processes
    vector<double> local_M(hs, 0.0);
    
    // Each process computes assigned M matrices based on rank
    if (rank % 7 == 0) {
        // M1 = (A11 + A22) * (B11 + B22)
        local_M = multiply(add(A11, A22, h), add(B11, B22, h), h, h, h);
    } else if (rank % 7 == 1) {
        // M2 = (A21 + A22) * B11
        local_M = multiply(add(A21, A22, h), B11, h, h, h);
    } else if (rank % 7 == 2) {
        // M3 = A11 * (B12 - B22)
        local_M = multiply(A11, sub(B12, B22, h), h, h, h);
    } else if (rank % 7 == 3) {
        // M4 = A22 * (B21 - B11)
        local_M = multiply(A22, sub(B21, B11, h), h, h, h);
    } else if (rank % 7 == 4) {
        // M5 = (A11 + A12) * B22
        local_M = multiply(add(A11, A12, h), B22, h, h, h);
    } else if (rank % 7 == 5) {
        // M6 = (A21 - A11) * (B11 + B12)
        local_M = multiply(sub(A21, A11, h), add(B11, B12, h), h, h, h);
    } else if (rank % 7 == 6) {
        // M7 = (A12 - A22) * (B21 + B22)
        local_M = multiply(sub(A12, A22, h), add(B21, B22, h), h, h, h);
    }
    
    // Gather all M matrices to rank 0
    vector<double> M1(hs), M2(hs), M3(hs), M4(hs), M5(hs), M6(hs), M7(hs);
    
    if (size >= 7) {
        // Collect results using point-to-point communication
        if (rank == 0) {
            M1 = local_M;
            MPI_Recv(M2.data(), hs, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M3.data(), hs, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M4.data(), hs, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M5.data(), hs, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M6.data(), hs, MPI_DOUBLE, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M7.data(), hs, MPI_DOUBLE, 6, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else if (rank >= 1 && rank <= 6) {
            MPI_Send(local_M.data(), hs, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    } else {
        // Not enough processes, compute sequentially on rank 0
        if (rank == 0) {
            M1 = multiply(add(A11, A22, h), add(B11, B22, h), h, h, h);
            M2 = multiply(add(A21, A22, h), B11, h, h, h);
            M3 = multiply(A11, sub(B12, B22, h), h, h, h);
            M4 = multiply(A22, sub(B21, B11, h), h, h, h);
            M5 = multiply(add(A11, A12, h), B22, h, h, h);
            M6 = multiply(sub(A21, A11, h), add(B11, B12, h), h, h, h);
            M7 = multiply(sub(A12, A22, h), add(B21, B22, h), h, h, h);
        }
    }
    
    // Combine results on rank 0
    vector<double> C(m * m);
    
    if (rank == 0) {
        // C11 = M1 + M4 - M5 + M7
        auto C11 = add(sub(add(M1, M4, h), M5, h), M7, h);
        // C12 = M3 + M5
        auto C12 = add(M3, M5, h);
        // C21 = M2 + M4
        auto C21 = add(M2, M4, h);
        // C22 = M1 + M3 - M2 + M6
        auto C22 = add(sub(add(M1, M3, h), M2, h), M6, h);
        
        // Reassemble the result matrix
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < h; j++) {
                C[i * m + j] = C11[i * h + j];
                C[i * m + j + h] = C12[i * h + j];
                C[(i + h) * m + j] = C21[i * h + j];
                C[(i + h) * m + j + h] = C22[i * h + j];
            }
        }
    }
    
    return C;
}