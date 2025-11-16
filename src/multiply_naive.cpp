#include "matrix.h"

Matrix Matrix::multiplyNaive(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument("Matrix dimensions incompatible for multiplication");
    }

    Matrix C(A.rows(), B.cols());

    // Classic triple-loop: O(n³)
    for (int i = 0; i < A.rows(); i++)
    {
        for (int j = 0; j < B.cols(); j++)
        {
            double sum = 0.0;
            for (int k = 0; k < A.cols(); k++)
            {
                sum += A(i, k) * B(k, j);
            }
            C(i, j) = sum;
        }
    }

    return C;
}
