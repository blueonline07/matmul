#include "matrix.h"
Matrix Matrix::multiplyNaive(const Matrix &A, const Matrix &B)
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
