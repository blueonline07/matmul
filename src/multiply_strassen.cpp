#include "matrix.h"

/**
 * Strassen's matrix multiplication algorithm
 * 
 * A divide-and-conquer algorithm that reduces multiplication count from 8 to 7
 * at each recursion level, achieving O(n^2.807) complexity vs O(n^3) for naive.
 * 
 * Algorithm:
 * 1. Pad matrices to square power-of-2 dimensions
 * 2. Divide each matrix into 4 quadrants
 * 3. Compute 7 products (M1-M7) using Strassen's formulas
 * 4. Combine products to form result quadrants
 * 5. Recurse until base case (use naive for small matrices)
 * 
 * Trade-offs:
 * - Faster for large matrices (n > 512 typically)
 * - Higher memory overhead (many temporary matrices)
 * - Slightly worse numerical stability due to more additions/subtractions
 * - Recursive overhead makes it slower for small matrices
 * 
 * Tuning parameters:
 * - STRASSEN_THRESHOLD: Switch to naive below this size (empirically tuned)
 * - Larger threshold = less recursion overhead, but loses Strassen benefit
 * - Optimal value depends on cache size, CPU architecture
 * 
 * Reference: Strassen, V. (1969). "Gaussian Elimination is not Optimal"
 */
Matrix Matrix::multiplyStrassen(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B))
    {
        throw std::invalid_argument(
            "Matrix dimensions incompatible for multiplication: " +
            std::to_string(A.rows()) + "x" + std::to_string(A.cols()) +
            " * " + std::to_string(B.rows()) + "x" + std::to_string(B.cols()));
    }

    // Pad matrices to next power of 2 for divide-and-conquer
    Matrix A_padded = A.padToPowerOf2();
    Matrix B_padded = B.padToPowerOf2();
    const int n = A_padded.rows(); // Now square n×n where n is power of 2

    // Base case: use optimized naive for small matrices
    // Empirically tuned threshold - balance between Strassen benefit and overhead
    // Below 128, naive is typically faster due to better cache behavior
    const int STRASSEN_THRESHOLD = 128;
    
    if (n <= STRASSEN_THRESHOLD)
    {
        // Use naive for base case (already optimized with ikj ordering)
        Matrix C_padded = multiplyNaive(A_padded, B_padded);
        return C_padded.removePadding(A.rows(), B.cols());
    }

    // Recursive case: divide and conquer
    const int k = n / 2;

    // Divide A into 4 quadrants: A = [A11 A12]
    //                                 [A21 A22]
    Matrix A11 = A_padded.submatrix(0, 0, k, k);
    Matrix A12 = A_padded.submatrix(0, k, k, k);
    Matrix A21 = A_padded.submatrix(k, 0, k, k);
    Matrix A22 = A_padded.submatrix(k, k, k, k);

    // Divide B into 4 quadrants: B = [B11 B12]
    //                                 [B21 B22]
    Matrix B11 = B_padded.submatrix(0, 0, k, k);
    Matrix B12 = B_padded.submatrix(0, k, k, k);
    Matrix B21 = B_padded.submatrix(k, 0, k, k);
    Matrix B22 = B_padded.submatrix(k, k, k, k);

    // Strassen's 7 products (instead of naive's 8)
    // M1 = (A11 + A22) * (B11 + B22)
    // M2 = (A21 + A22) * B11
    // M3 = A11 * (B12 - B22)
    // M4 = A22 * (B21 - B11)
    // M5 = (A11 + A12) * B22
    // M6 = (A21 - A11) * (B11 + B12)
    // M7 = (A12 - A22) * (B21 + B22)
    Matrix M1 = multiplyStrassen(A11 + A22, B11 + B22);
    Matrix M2 = multiplyStrassen(A21 + A22, B11);
    Matrix M3 = multiplyStrassen(A11, B12 - B22);
    Matrix M4 = multiplyStrassen(A22, B21 - B11);
    Matrix M5 = multiplyStrassen(A11 + A12, B22);
    Matrix M6 = multiplyStrassen(A21 - A11, B11 + B12);
    Matrix M7 = multiplyStrassen(A12 - A22, B21 + B22);

    // Combine products to form result quadrants
    // C11 = M1 + M4 - M5 + M7
    // C12 = M3 + M5
    // C21 = M2 + M4
    // C22 = M1 - M2 + M3 + M6
    Matrix C11 = M1 + M4 - M5 + M7;
    Matrix C12 = M3 + M5;
    Matrix C21 = M2 + M4;
    Matrix C22 = M1 - M2 + M3 + M6;

    // Assemble result from quadrants
    Matrix C_padded(n, n);
    C_padded.copySubmatrix(C11, 0, 0);
    C_padded.copySubmatrix(C12, 0, k);
    C_padded.copySubmatrix(C21, k, 0);
    C_padded.copySubmatrix(C22, k, k);

    // Remove padding to return original result dimensions
    return C_padded.removePadding(A.rows(), B.cols());
}