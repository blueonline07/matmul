#include "matrix.h"
#include <mpi.h>
#include <omp.h>

/**
 * MPI-based distributed matrix multiplication
 * 
 * TODO: Implementation required for assignment
 * 
 * Recommended approach:
 * 1. Row-wise block distribution of matrix A
 * 2. Broadcast matrix B to all processes (MPI_Bcast)
 * 3. Each process computes C_local = A_local * B
 * 4. Gather results to root process (MPI_Gather)
 * 
 * Key considerations:
 * - Handle non-divisible matrix dimensions (remainder rows)
 * - Use MPI_Scatterv for uneven distribution
 * - Ensure MPI_Init/MPI_Finalize are called in main
 * - Measure communication overhead separately for analysis
 * 
 * Alternative approaches:
 * - Cannon's algorithm (2D block distribution)
 * - Fox's algorithm (optimal for square process grids)
 * - SUMMA (Scalable Universal Matrix Multiply Algorithm)
 * 
 * Performance expectations:
 * - Speedup limited by communication overhead
 * - Efficiency typically 60-80% with 4-8 processes
 * - Best for large matrices (n > 2000)
 */
Matrix Matrix::multiplyMPI(const Matrix &A, const Matrix &B)
{
    // Temporary fallback: use optimized naive implementation
    // This allows tests to pass while MPI is being implemented
    return multiplyNaive(A, B);
}

/**
 * MPI-based Strassen's algorithm
 * 
 * TODO: Implementation pending
 * 
 * Strategy:
 * - Distribute Strassen's 7 products (M1-M7) across MPI processes
 * - Each process computes one or more of the 7 products
 * - Use MPI_Reduce or MPI_Gather to combine results
 * - Handle recursive calls: distribute sub-problems to processes
 * 
 * Implementation approaches:
 * 1. **Product-level distribution**: Assign each of 7 products to different processes
 *    - Requires 7+ processes for full parallelism
 *    - Simple but limited scalability
 * 
 * 2. **Recursive distribution**: Distribute quadrants at each recursion level
 *    - Better scalability for many processes
 *    - More complex communication pattern
 * 
 * 3. **Hybrid approach**: Combine product-level and recursive distribution
 *    - Optimal for large matrices and many processes
 * 
 * Key considerations:
 * - Strassen requires power-of-2 dimensions (already handled by padding)
 * - Communication overhead increases with recursion depth
 * - Load balancing: 7 products have different computational costs
 * - Best for very large matrices (n > 4096) with 7+ processes
 * 
 * Performance expectations:
 * - Speedup limited by communication and load imbalance
 * - Efficiency: 40-60% with 7-14 processes
 * - Best when computation >> communication
 */
Matrix Matrix::multiplyStrassenMPI(const Matrix &A, const Matrix &B)
{
    // Temporary fallback: use sequential Strassen implementation
    // This allows tests to pass while MPI Strassen is being implemented
    return multiplyStrassen(A, B);
}

/**
 * Hybrid MPI+OpenMP Strassen's algorithm
 * 
 * TODO: Implementation pending
 * 
 * Strategy:
 * - Combine MPI distribution with OpenMP threading for Strassen's algorithm
 * - MPI: Distribute work across nodes (inter-node parallelism)
 * - OpenMP: Parallelize within each node (intra-node parallelism)
 * - Use MPI_Init_thread with MPI_THREAD_FUNNELED or MPI_THREAD_SERIALIZED
 * 
 * Implementation structure:
 * ```cpp
 * // MPI level: distribute 7 products across processes
 * if (my_rank == 0) {
 *     // Assign M1-M7 to different processes
 * }
 * 
 * // OpenMP level: within each process, parallelize computation
 * #pragma omp parallel
 * {
 *     #pragma omp task
 *     compute_M1();
 *     // ... other tasks
 * }
 * ```
 * 
 * Key considerations:
 * - Thread safety: Ensure MPI calls are thread-safe
 * - Task granularity: Balance between MPI and OpenMP parallelism
 * - Nested parallelism: OpenMP tasks within MPI processes
 * - Optimal for multi-node clusters with multi-core nodes
 * 
 * Performance expectations:
 * - Best performance on clusters (4+ nodes × 8+ cores)
 * - Expected speedup: (nodes × cores) × efficiency
 * - Efficiency: 30-50% due to combined overheads
 * - Example: 4 nodes × 8 cores = 32× theoretical, 10-15× realistic
 * 
 * Testing:
 * - Run with: mpirun -np 7 ./bin/performance (7 processes for 7 products)
 * - Set OMP_NUM_THREADS=8 for 8 OpenMP threads per process
 * - Total parallelism = MPI_processes × OMP_threads
 */
Matrix Matrix::multiplyStrassenHybrid(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B)) {
        throw std::invalid_argument("Incompatible dimensions for multiplication (A.cols != B.rows)");
    }

    int world_size = 1, world_rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // If only one MPI rank, use your sequential Strassen (assumed implemented)
    if (world_size == 1) {
        return multiplyStrassen(A, B); // use existing sequential Strassen implementation
    }

    // Otherwise, for robustness we perform row-wise distribution with OpenMP compute (fallback).
    // This mirrors multiplyHybrid's behavior but keeps the API semantics.
    int A_rows = A.rows_;
    int A_cols = A.cols_;
    int B_cols = B.cols_;

    int base = A_rows / world_size;
    int rem  = A_rows % world_size;
    std::vector<int> counts(world_size), displs(world_size);
    for (int r = 0; r < world_size; ++r) {
        counts[r] = (r < rem) ? (base + 1) : base;
        displs[r] = (r == 0) ? 0 : displs[r-1] + counts[r-1];
    }
    int my_rows = counts[world_rank];

    std::vector<int> sendcounts(world_size), senddispls(world_size);
    for (int r = 0; r < world_size; ++r) {
        sendcounts[r] = counts[r] * A_cols;
        senddispls[r] = displs[r] * A_cols;
    }

    std::vector<double> A_local(my_rows * A_cols);
    std::vector<double> B_buf(A_cols * B_cols);
    std::vector<double> C_local(my_rows * B_cols, 0.0);

    const double *A_data_ptr = A.data_.data();
    const double *B_data_ptr = B.data_.data();

    MPI_Scatterv((world_rank==0 ? A_data_ptr : nullptr), sendcounts.data(), senddispls.data(), MPI_DOUBLE,
                 (my_rows>0 ? A_local.data() : nullptr), my_rows * A_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    MPI_Bcast((world_rank==0 ? const_cast<double*>(B_data_ptr) : B_buf.data()),
              A_cols * B_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (world_rank == 0) {
        std::copy(B_data_ptr, B_data_ptr + (A_cols * B_cols), B_buf.data());
    }

    // Compute local product using OpenMP parallel loops (simple and robust fallback)
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < my_rows; ++i) {
        for (int j = 0; j < B_cols; ++j) {
            C_local[i * B_cols + j] = 0.0;
        }
        for (int k = 0; k < A_cols; ++k) {
            double a_ik = A_local[i * A_cols + k];
            const double *brow = &B_buf[k * B_cols];
            double *crow = &C_local[i * B_cols];
            for (int j = 0; j < B_cols; ++j) {
                crow[j] += a_ik * brow[j];
            }
        }
    }

    // Allgather to make full C available on all ranks (keeps return semantics)
    std::vector<int> recvcounts(world_size), recvdispls(world_size);
    for (int r=0; r<world_size; ++r) {
        recvcounts[r] = counts[r] * B_cols;
        recvdispls[r] = displs[r] * B_cols;
    }

    std::vector<double> C_full(A_rows * B_cols);
    MPI_Allgatherv((my_rows>0 ? C_local.data() : nullptr), my_rows * B_cols, MPI_DOUBLE,
                   C_full.data(), recvcounts.data(), recvdispls.data(), MPI_DOUBLE,
                   MPI_COMM_WORLD);

    Matrix C(A_rows, B_cols);
    std::copy(C_full.begin(), C_full.end(), C.data_.begin());
    return C;
}
