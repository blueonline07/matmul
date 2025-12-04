#include "matrix.h"

Matrix Matrix::multiplyStrassen(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument(Matrix::INCOMPATIBLE_DIMENSIONS_MSG);
    }

    Matrix A_padded = A.padToPowerOf2();
    Matrix B_padded = B.padToPowerOf2();
    const int n = A_padded.rows();

    const int STRASSEN_THRESHOLD = 128;
    
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

    Matrix M1 = multiplyStrassen(A11 + A22, B11 + B22);
    Matrix M2 = multiplyStrassen(A21 + A22, B11);
    Matrix M3 = multiplyStrassen(A11, B12 - B22);
    Matrix M4 = multiplyStrassen(A22, B21 - B11);
    Matrix M5 = multiplyStrassen(A11 + A12, B22);
    Matrix M6 = multiplyStrassen(A21 - A11, B11 + B12);
    Matrix M7 = multiplyStrassen(A12 - A22, B21 + B22);

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
