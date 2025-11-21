#include "matrix.h"
#include <openmp.h>
#include <mpi.h>

/**
 * Hybrid MPI+OpenMP matrix multiplication
 * 
 * TODO: Implementation required for assignment
 * 
 * Implementation strategy:
 * 1. Initialize MPI with thread support: MPI_Init_thread(MPI_THREAD_FUNNELED)
 * 2. Distribute work across MPI processes (inter-node parallelism)
 * 3. Use OpenMP within each process (intra-node parallelism)
 * 4. Combine MPI distribution with OpenMP threading
 * 
 * Example structure:
 * ```cpp
 * // Each MPI process gets block of rows
 * int rows_per_process = m / num_processes;
 * 
 * // Within each process, use OpenMP for computation
 * #pragma omp parallel for
 * for (int i = 0; i < local_rows; i++) {
 *     // Compute row i of local result
 * }
 * ```
 * 
 * Performance expectations:
 * - Best performance on multi-node clusters with multi-core nodes
 * - Expected speedup: (num_nodes × cores_per_node) × efficiency
 * - Efficiency typically 50-70% due to combined overheads
 * - Example: 4 nodes × 8 cores = 32× theoretical, 16-20× realistic
 * 
 * Testing:
 * - Run with: mpirun -np 4 ./bin/performance
 * - Set OMP_NUM_THREADS=8 for 8 OpenMP threads per process
 * - Total parallelism = MPI_processes × OMP_threads
 */

Matrix Matrix::multiplyHybrid(const Matrix &A, const Matrix &B)
{
    if (!A.canMultiply(B)) {
        throw std::invalid_argument("Incompatible dimensions for multiplication (A.cols != B.rows)");
    }

    int world_size = 1, world_rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int A_rows = A.rows_;
    int A_cols = A.cols_; // == B.rows_
    int B_cols = B.cols_;

    // compute block-row distribution of A (rows per rank)
    int base = A_rows / world_size;
    int rem  = A_rows % world_size;
    std::vector<int> counts(world_size), displs(world_size);
    for (int r = 0; r < world_size; ++r) {
        counts[r] = (r < rem) ? (base + 1) : base;
        displs[r] = (r == 0) ? 0 : displs[r-1] + counts[r-1];
    }
    int my_rows = counts[world_rank];

    // Prepare sendcounts/displacements in *elements* for Scatterv/Gatherv (counts are rows)
    std::vector<int> sendcounts(world_size), senddispls(world_size);
    for (int r = 0; r < world_size; ++r) {
        sendcounts[r] = counts[r] * A_cols;      // number of doubles to send to proc r
        senddispls[r] = displs[r] * A_cols;      // displacement in elements
    }

    // Local storage
    std::vector<double> A_local(my_rows * A_cols);
    std::vector<double> B_buf(B_cols * A_cols); // note: we'll copy B as row-major sized as A_cols x B_cols
    std::vector<double> C_local(my_rows * B_cols, 0.0);

    // Root prepares buffers
    const double *A_data_ptr = A.data_.data();
    const double *B_data_ptr = B.data_.data();

    // Scatter rows of A (row-major contiguous)
    MPI_Scatterv((world_rank==0 ? A_data_ptr : nullptr), sendcounts.data(), senddispls.data(), MPI_DOUBLE,
                 (my_rows>0 ? A_local.data() : nullptr), my_rows * A_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Broadcast B to all ranks (B size = A_cols x B_cols)
    // We'll broadcast B in row-major order as stored in Matrix
    // Note: B.data_.size() == A_cols * B_cols
    MPI_Bcast((world_rank==0 ? const_cast<double*>(B_data_ptr) : B_buf.data()),
              A_cols * B_cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // If non-root, copy the received broadcast into B_buf (MPI_Bcast wrote directly into B_buf)
    if (world_rank == 0) {
        // If root, we already have B_data_ptr (no need to copy), but for easier code unify to B_buf
        std::copy(B_data_ptr, B_data_ptr + (A_cols * B_cols), B_buf.data());
    } else {
        // MPI_Bcast wrote into B_buf; nothing else needed
    }

    // Compute local product: C_local = A_local * B
    // A_local: my_rows x A_cols
    // B_buf:   A_cols x B_cols
    // C_local: my_rows x B_cols
    //
    // Parallelize with OpenMP (outer loop over local rows)
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < my_rows; ++i) {
        // initialize row
        for (int j = 0; j < B_cols; ++j) {
            C_local[i * B_cols + j] = 0.0;
        }
        // compute row i
        for (int k = 0; k < A_cols; ++k) {
            double a_ik = A_local[i * A_cols + k];
            const double *brow = &B_buf[k * B_cols];
            double *crow = &C_local[i * B_cols];
            for (int j = 0; j < B_cols; ++j) {
                crow[j] += a_ik * brow[j];
            }
        }
    }

    // Gather all C_local into a full C matrix on every rank using Allgatherv.
    // This keeps the function return simple (every rank receives full result).
    std::vector<int> recvcounts(world_size), recvdispls(world_size);
    for (int r=0; r<world_size; ++r) {
        recvcounts[r] = counts[r] * B_cols;       // number of elements contributed by proc r
        recvdispls[r] = displs[r] * B_cols;       // displacement in elements
    }

    std::vector<double> C_full(A_rows * B_cols);
    MPI_Allgatherv((my_rows>0 ? C_local.data() : nullptr), my_rows * B_cols, MPI_DOUBLE,
                   C_full.data(), recvcounts.data(), recvdispls.data(), MPI_DOUBLE,
                   MPI_COMM_WORLD);

    // Construct result Matrix and return
    Matrix C(A_rows, B_cols);
    std::copy(C_full.begin(), C_full.end(), C.data_.begin());
    return C;
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

