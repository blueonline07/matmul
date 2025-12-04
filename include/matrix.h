#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <vector>
#include <memory>
#include <cmath>
#include <cstring>
#include <stdexcept>

/**
 * Matrix class using contiguous memory (flat array) for performance
 * we use a flat vector for cache-friendly access
 * data[i * cols + j] accesses element at row i, column j
 */
class Matrix
{
private:
    int rows_;
    int cols_;
    std::vector<double> data_;

public:
    /**
     * Error message for incompatible dimensions in multiplication
     */
    static constexpr const char *INCOMPATIBLE_DIMENSIONS_MSG = "Incompatible dimensions for multiplication (A.cols != B.rows)";

    /* ========================================================================
     * CONSTRUCTORS & DESTRUCTOR
     * ===================================================================== */

    /**
     * Create a matrix with given dimensions, initialized to zero
     */
    Matrix(int rows, int cols);

    /**
     * Copy constructor
     */
    Matrix(const Matrix &other);

    /**
     * Move constructor
     */
    Matrix(Matrix &&other) noexcept;

    /**
     * Copy assignment operator
     */
    Matrix &operator=(const Matrix &other);

    /**
     * Move assignment operator
     */
    Matrix &operator=(Matrix &&other) noexcept;

    /**
     * Destructor (default is fine with vector)
     */
    ~Matrix() = default;

    /* ========================================================================
     * ACCESSORS
     * ===================================================================== */

    /**
     * Get number of rows
     */
    int rows() const { return rows_; }

    /**
     * Get number of columns
     */
    int cols() const { return cols_; }

    /**
     * Get total number of elements
     */
    int size() const { return rows_ * cols_; }

    /**
     * Access element at (i, j) - const version
     */
    double operator()(int i, int j) const;

    /**
     * Access element at (i, j) - non-const version
     */
    double &operator()(int i, int j);

    /**
     * Get raw pointer to data (for MPI/performance-critical code)
     */
    double *data() { return data_.data(); }
    const double *data() const { return data_.data(); }

    /* ========================================================================
     * INITIALIZATION METHODS
     * ===================================================================== */

    /**
     * Initialize with random values in [0, 1]
     */
    void initRandom(unsigned int seed = 42);

    /**
     * Initialize as identity matrix (must be square)
     */
    void initIdentity();

    /**
     * Initialize all elements to a constant value
     */
    void initConstant(double value);

    /**
     * Initialize to zeros
     */
    void initZeros();

    /* ========================================================================
     * ARITHMETIC OPERATORS
     * ===================================================================== */

    /**
     * Matrix addition: C = A + B
     */
    Matrix operator+(const Matrix &other) const;

    /**
     * Matrix subtraction: C = A - B
     */
    Matrix operator-(const Matrix &other) const;

    /**
     * Scalar multiplication: C = alpha * A
     */
    Matrix operator*(double scalar) const;

    /**
     * In-place addition
     */
    Matrix &operator+=(const Matrix &other);

    /**
     * In-place subtraction
     */
    Matrix &operator-=(const Matrix &other);

    /**
     * In-place scalar multiplication
     */
    Matrix &operator*=(double scalar);

    /* ========================================================================
     * MATRIX MULTIPLICATION METHODS
     * ===================================================================== */

    /**
     * Naive matrix multiplication: C = A * B
     * Standard O(n³) triple-loop implementation
     */
    static Matrix multiplyNaive(const Matrix &A, const Matrix &B);

    /**
     * Strassen's matrix multiplication algorithm
     * Divide-and-conquer approach with O(n^2.807) complexity
     */
    static Matrix multiplyStrassen(const Matrix &A, const Matrix &B);

    /**
     * OpenMP parallelized naive matrix multiplication
     */
    static Matrix multiplyOpenMP(const Matrix &A, const Matrix &B);

    /**
     * OpenMP parallelized Strassen's algorithm
     * Combines Strassen's divide-and-conquer with OpenMP threading
     * TODO: Implementation pending
     */
    static Matrix multiplyStrassenOpenMP(const Matrix &A, const Matrix &B);

    /**
     * MPI-based matrix multiplication
     * Handles MPI internally - same interface as other methods
     */
    static Matrix multiplyMPI(const Matrix &A, const Matrix &B, int size, int rank);

    /**
     * MPI-based Strassen's algorithm
     * Distributed Strassen's algorithm using MPI
     * TODO: Implementation pending
     */
    static Matrix multiplyStrassenMPI(const Matrix &A, const Matrix &B, int size, int rank);

    /**
     * Hybrid MPI+OpenMP matrix multiplication
     * Handles MPI internally - same interface as other methods
     */
    static Matrix multiplyHybrid(const Matrix &A, const Matrix &B, int size, int rank);

    /**
     * Hybrid MPI+OpenMP Strassen's algorithm
     * Combines MPI distribution with OpenMP threading for Strassen's algorithm
     * TODO: Implementation pending
     */
    static Matrix multiplyStrassenHybrid(const Matrix &A, const Matrix &B, int size, int rank);

    /* ========================================================================
     * UTILITY METHODS
     * ===================================================================== */

    /**
     * Check if two matrices have the same dimensions
     */
    bool sameDimensions(const Matrix &other) const;

    /**
     * Check if this matrix can multiply with another (this.cols == other.rows)
     */
    bool canMultiply(const Matrix &other) const;

    /**
     * Compare matrices for equality within tolerance
     */
    bool equals(const Matrix &other, double tolerance = 1e-9) const;

    /**
     * Calculate relative error compared to reference matrix
     * Error = ||this - ref||_F / ||ref||_F
     */
    double relativeError(const Matrix &reference) const;

    /**
     * Print matrix (for debugging/small matrices)
     */
    void print(const std::string &label = "Matrix") const;

    /**
     * Extract submatrix
     */
    Matrix submatrix(int rowStart, int colStart, int numRows, int numCols) const;

    /**
     * Copy submatrix into this matrix at given offset
     */
    void copySubmatrix(const Matrix &src, int rowOffset, int colOffset);

    /**
     * Pad to next power of 2 dimensions (for Strassen)
     */
    Matrix padToPowerOf2() const;

    /**
     * Remove padding (extract top-left submatrix)
     */
    Matrix removePadding(int origRows, int origCols) const;

    /* ========================================================================
     * STATIC UTILITY FUNCTIONS
     * ===================================================================== */

    /**
     * Check if number is power of 2
     */
    static bool isPowerOf2(int n);

    /**
     * Find next power of 2
     */
    static int nextPowerOf2(int n);


};

/**
 * Scalar multiplication (left side): C = scalar * A
 */
Matrix operator*(double scalar, const Matrix &m);

/**
 * Stream output operator for easy printing
 */
std::ostream &operator<<(std::ostream &os, const Matrix &m);

#endif // MATRIX_H