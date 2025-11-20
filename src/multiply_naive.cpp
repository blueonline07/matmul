#include "matrix.h"

/**
 * Naive matrix multiplication with optimized loop ordering
 * 
 * Uses ikj loop ordering instead of ijk for better cache performance:
 * - i-loop: iterate over rows of A and C (row-major, cache-friendly)
 * - k-loop: iterate over columns of A / rows of B
 * - j-loop: iterate over columns of B and C (inner loop, vectorizable)
 * 
 * This ordering ensures:
 * 1. A(i,k) is loaded once per k iteration (cached)
 * 2. B(k,j) and C(i,j) are accessed row-wise (sequential, cache-friendly)
 * 3. Better spatial locality for both reads and writes
 * 
 * Complexity: O(m*n*k) where A is m×k and B is k×n
 */
Matrix Matrix::multiplyNaive(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument(
            "Matrix dimensions incompatible for multiplication: " +
            std::to_string(A.rows()) + "x" + std::to_string(A.cols()) +
            " * " + std::to_string(B.rows()) + "x" + std::to_string(B.cols()));
    }

    const int m = A.rows();
    const int n = B.cols();
    const int k = A.cols();

    Matrix C(m, n);
    C.initZeros(); // Ensure zero initialization for accumulation

    // ikj loop ordering for optimal cache performance
    for (int i = 0; i < m; i++)
    {
        for (int k_idx = 0; k_idx < k; k_idx++)
        {
            // Cache A(i, k_idx) in register
            const double a_ik = A(i, k_idx);
            
            // Vectorizable inner loop with row-wise access
            for (int j = 0; j < n; j++)
            {
                C(i, j) += a_ik * B(k_idx, j);
            }
        }
    }

    return C;
}
