#include "matrix.h"
#include <omp.h>

/**
 * OpenMP-parallelized matrix multiplication with optimized loop ordering
 * 
 * Key optimizations:
 * 1. Parallelize only the outer i-loop to minimize thread synchronization overhead
 * 2. Use ikj loop ordering for cache-friendly memory access (same as naive optimized)
 * 3. Dynamic scheduling for better load balancing across threads
 * 4. Each thread works on complete rows, maximizing cache reuse
 * 
 * Performance characteristics:
 * - Near-linear speedup up to number of physical cores
 * - Good cache locality within each thread
 * - Minimal false sharing (each thread writes to different rows)
 * 
 * Environment variables:
 * - OMP_NUM_THREADS: Set number of threads (default: number of CPU cores)
 * - OMP_SCHEDULE: Override scheduling policy if needed
 */
Matrix Matrix::multiplyOpenMP(const Matrix &A, const Matrix &B)
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
    C.initZeros();

    // Parallelize outer loop only - each thread handles complete rows
    // Dynamic scheduling provides better load balancing than static
    #pragma omp parallel for schedule(dynamic, 8)
    for (int i = 0; i < m; i++)
    {
        // ikj ordering: inner loops are cache-friendly and vectorizable
        for (int k_idx = 0; k_idx < k; k_idx++)
        {
            const double a_ik = A(i, k_idx);
            
            // Innermost loop: sequential memory access, compiler can vectorize
            for (int j = 0; j < n; j++)
            {
                C(i, j) += a_ik * B(k_idx, j);
            }
        }
    }

    return C;
}

/**
 * OpenMP-parallelized Strassen's algorithm
 * 
 * TODO: Implementation pending
 * 
 * Strategy:
 * - Parallelize the 7 recursive Strassen multiplications using OpenMP tasks
 * - Use task-based parallelism: #pragma omp task for each M1-M7 computation
 * - Each task can be executed in parallel by different threads
 * - Synchronize with #pragma omp taskwait before combining results
 * 
 * Alternative approach:
 * - Parallelize at the quadrant level (4 quadrants × 7 products = 28 tasks)
 * - Use nested parallelism for recursive calls
 * - Tune task granularity based on matrix size
 * 
 * Performance expectations:
 * - Better than sequential Strassen for large matrices (n > 512)
 * - Speedup depends on recursion depth and thread count
 * - Overhead from task creation may limit speedup for small submatrices
 * - Expected: 2-4× speedup on 8-core CPU (vs sequential Strassen)
 */
Matrix Matrix::multiplyStrassenOpenMP(const Matrix &A, const Matrix &B)
{
    // Temporary fallback: use sequential Strassen implementation
    // This allows tests to pass while OpenMP Strassen is being implemented
    return multiplyStrassen(A, B);
}
