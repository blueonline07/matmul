#include "matrix.h"
#include <omp.h>
Matrix Matrix::multiplyOpenMP(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument(Matrix::INCOMPATIBLE_DIMENSIONS_MSG);
    }

    const int m = A.rows();
    const int n = B.cols();
    const int k = A.cols();

    Matrix C(m, n);
    C.initZeros();
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < m; i++)
    {
        for (int k_idx = 0; k_idx < k; k_idx++)
        {
            const double a_ik = A(i, k_idx);
            for (int j = 0; j < n; j++)
            {
                C(i, j) += a_ik * B(k_idx, j);
            }
        }
    }

    return C;
}

Matrix Matrix::multiplyStrassenOpenMP(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument(Matrix::INCOMPATIBLE_DIMENSIONS_MSG);
    }


    Matrix A_padded = A.padToPowerOf2();
    Matrix B_padded = B.padToPowerOf2();
    const int n = A_padded.rows();
    const int STRASSEN_THRESHOLD = 128;
    const int TASK_CUTOFF = 512;
    
    if (n <= STRASSEN_THRESHOLD)
    {
        Matrix C_padded = multiplyNaive(A_padded, B_padded);
        return C_padded.removePadding(A.rows(), B.cols());
    }

    const int k = n / 2;

    Matrix A11 = A_padded.submatrix(0, 0, k, k);
    Matrix A12 = A_padded.submatrix(0, k, k, k);
    Matrix A21 = A_padded.submatrix(k, 0, k, k);
    Matrix A22 = A_padded.submatrix(k, k, k, k);

    Matrix B11 = B_padded.submatrix(0, 0, k, k);
    Matrix B12 = B_padded.submatrix(0, k, k, k);
    Matrix B21 = B_padded.submatrix(k, 0, k, k);
    Matrix B22 = B_padded.submatrix(k, k, k, k);

    Matrix M1(k, k), M2(k, k), M3(k, k), M4(k, k), M5(k, k), M6(k, k), M7(k, k);
    
    if (n >= TASK_CUTOFF)
    {
        #pragma omp parallel
        {
            #pragma omp single nowait
            {
                #pragma omp task shared(M1)
                M1 = multiplyStrassenOpenMP(A11 + A22, B11 + B22);
                
                #pragma omp task shared(M2)
                M2 = multiplyStrassenOpenMP(A21 + A22, B11);
                
                #pragma omp task shared(M3)
                M3 = multiplyStrassenOpenMP(A11, B12 - B22);
                
                #pragma omp task shared(M4)
                M4 = multiplyStrassenOpenMP(A22, B21 - B11);
                
                #pragma omp task shared(M5)
                M5 = multiplyStrassenOpenMP(A11 + A12, B22);
                
                #pragma omp task shared(M6)
                M6 = multiplyStrassenOpenMP(A21 - A11, B11 + B12);
                
                #pragma omp task shared(M7)
                M7 = multiplyStrassenOpenMP(A12 - A22, B21 + B22);
                
                #pragma omp taskwait
            }
        }
    }
    else
    {
        M1 = multiplyStrassen(A11 + A22, B11 + B22);
        M2 = multiplyStrassen(A21 + A22, B11);
        M3 = multiplyStrassen(A11, B12 - B22);
        M4 = multiplyStrassen(A22, B21 - B11);
        M5 = multiplyStrassen(A11 + A12, B22);
        M6 = multiplyStrassen(A21 - A11, B11 + B12);
        M7 = multiplyStrassen(A12 - A22, B21 + B22);
    }

    Matrix C11 = M1 + M4 - M5 + M7;
    Matrix C12 = M3 + M5;
    Matrix C21 = M2 + M4;
    Matrix C22 = M1 - M2 + M3 + M6;

    Matrix C_padded(n, n);
    C_padded.copySubmatrix(C11, 0, 0);
    C_padded.copySubmatrix(C12, 0, k);
    C_padded.copySubmatrix(C21, k, 0);
    C_padded.copySubmatrix(C22, k, k);

    return C_padded.removePadding(A.rows(), B.cols());
}
