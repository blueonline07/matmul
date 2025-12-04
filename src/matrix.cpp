#include "matrix.h"
#include <random>
#include <iomanip>
#include <algorithm>

/* ============================================================================
 * CONSTRUCTORS & DESTRUCTOR
 * ========================================================================= */

Matrix::Matrix(int rows, int cols)
    : rows_(rows), cols_(cols), data_(rows * cols, 0.0)
{
    if (rows <= 0 || cols <= 0)
    {
        throw std::invalid_argument("Matrix dimensions must be positive");
    }
}

Matrix::Matrix(const Matrix &other)
    : rows_(other.rows_), cols_(other.cols_), data_(other.data_)
{
}

Matrix::Matrix(Matrix &&other) noexcept
    : rows_(other.rows_), cols_(other.cols_), data_(std::move(other.data_))
{
    other.rows_ = 0;
    other.cols_ = 0;
}

Matrix &Matrix::operator=(const Matrix &other)
{
    if (this != &other)
    {
        rows_ = other.rows_;
        cols_ = other.cols_;
        data_ = other.data_;
    }
    return *this;
}

Matrix &Matrix::operator=(Matrix &&other) noexcept
{
    if (this != &other)
    {
        rows_ = other.rows_;
        cols_ = other.cols_;
        data_ = std::move(other.data_);
        other.rows_ = 0;
        other.cols_ = 0;
    }
    return *this;
}

/* ============================================================================
 * ACCESSORS
 * ========================================================================= */

double Matrix::operator()(int i, int j) const
{
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_)
    {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data_[i * cols_ + j];
}

double &Matrix::operator()(int i, int j)
{
    if (i < 0 || i >= rows_ || j < 0 || j >= cols_)
    {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return data_[i * cols_ + j];
}

/* ============================================================================
 * INITIALIZATION METHODS
 * ========================================================================= */

void Matrix::initRandom(unsigned int seed)
{
    std::mt19937 gen(seed);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (auto &elem : data_)
    {
        elem = dist(gen);
    }
}

void Matrix::initIdentity()
{
    if (rows_ != cols_)
    {
        throw std::logic_error("Identity matrix must be square");
    }

    initZeros();
    for (int i = 0; i < rows_; i++)
    {
        (*this)(i, i) = 1.0;
    }
}

void Matrix::initConstant(double value)
{
    std::fill(data_.begin(), data_.end(), value);
}

void Matrix::initZeros()
{
    initConstant(0.0);
}

/* ============================================================================
 * ARITHMETIC OPERATORS
 * ========================================================================= */

Matrix Matrix::operator+(const Matrix &other) const
{
    if (!sameDimensions(other))
    {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }

    Matrix result(rows_, cols_);
    for (int i = 0; i < size(); i++)
    {
        result.data_[i] = data_[i] + other.data_[i];
    }
    return result;
}

Matrix Matrix::operator-(const Matrix &other) const
{
    if (!sameDimensions(other))
    {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }

    Matrix result(rows_, cols_);
    for (int i = 0; i < size(); i++)
    {
        result.data_[i] = data_[i] - other.data_[i];
    }
    return result;
}

Matrix Matrix::operator*(double scalar) const
{
    Matrix result(rows_, cols_);
    for (int i = 0; i < size(); i++)
    {
        result.data_[i] = data_[i] * scalar;
    }
    return result;
}

Matrix &Matrix::operator+=(const Matrix &other)
{
    if (!sameDimensions(other))
    {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }

    for (int i = 0; i < size(); i++)
    {
        data_[i] += other.data_[i];
    }
    return *this;
}

Matrix &Matrix::operator-=(const Matrix &other)
{
    if (!sameDimensions(other))
    {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }

    for (int i = 0; i < size(); i++)
    {
        data_[i] -= other.data_[i];
    }
    return *this;
}

Matrix &Matrix::operator*=(double scalar)
{
    for (auto &elem : data_)
    {
        elem *= scalar;
    }
    return *this;
}

/* ============================================================================
 * UTILITY METHODS
 * ========================================================================= */

bool Matrix::sameDimensions(const Matrix &other) const
{
    return rows_ == other.rows_ && cols_ == other.cols_;
}

bool Matrix::canMultiply(const Matrix &other) const
{
    return cols_ == other.rows_;
}

bool Matrix::equals(const Matrix &other, double tolerance) const
{
    if (!sameDimensions(other))
    {
        return false;
    }

    for (int i = 0; i < size(); i++)
    {
        if (std::abs(data_[i] - other.data_[i]) > tolerance)
        {
            return false;
        }
    }
    return true;
}

double Matrix::relativeError(const Matrix &reference) const
{
    if (!sameDimensions(reference))
    {
        throw std::invalid_argument("Matrices must have same dimensions");
    }

    double normDiff = 0.0;
    double normRef = 0.0;

    for (int i = 0; i < size(); i++)
    {
        double diff = data_[i] - reference.data_[i];
        normDiff += diff * diff;
        normRef += reference.data_[i] * reference.data_[i];
    }

    if (normRef < 1e-10)
    {
        return -1.0; // Reference is zero
    }

    return std::sqrt(normDiff) / std::sqrt(normRef);
}

void Matrix::print(const std::string &label) const
{
    std::cout << label << " (" << rows_ << "x" << cols_ << "):" << std::endl;

    // Only print small matrices fully
    int maxDisplay = 10;
    int displayRows = std::min(rows_, maxDisplay);
    int displayCols = std::min(cols_, maxDisplay);

    for (int i = 0; i < displayRows; i++)
    {
        for (int j = 0; j < displayCols; j++)
        {
            std::cout << std::setw(10) << std::fixed << std::setprecision(4)
                      << (*this)(i, j) << " ";
        }
        if (cols_ > maxDisplay)
        {
            std::cout << "...";
        }
        std::cout << std::endl;
    }
    if (rows_ > maxDisplay)
    {
        std::cout << "..." << std::endl;
    }
}

Matrix Matrix::submatrix(int rowStart, int colStart, int numRows, int numCols) const
{
    if (rowStart < 0 || colStart < 0 ||
        rowStart + numRows > rows_ || colStart + numCols > cols_)
    {
        throw std::out_of_range("Submatrix bounds out of range");
    }

    Matrix sub(numRows, numCols);
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            sub(i, j) = (*this)(rowStart + i, colStart + j);
        }
    }
    return sub;
}

void Matrix::copySubmatrix(const Matrix &src, int rowOffset, int colOffset)
{
    if (rowOffset < 0 || colOffset < 0 ||
        rowOffset + src.rows_ > rows_ || colOffset + src.cols_ > cols_)
    {
        throw std::out_of_range("Submatrix destination out of range");
    }

    for (int i = 0; i < src.rows_; i++)
    {
        for (int j = 0; j < src.cols_; j++)
        {
            (*this)(rowOffset + i, colOffset + j) = src(i, j);
        }
    }
}

Matrix Matrix::padToPowerOf2() const
{
    int maxDim = std::max(rows_, cols_);
    int newSize = nextPowerOf2(maxDim);

    if (newSize == rows_ && newSize == cols_)
    {
        return *this; // Already power of 2
    }

    Matrix padded(newSize, newSize);
    padded.initZeros();
    padded.copySubmatrix(*this, 0, 0);

    return padded;
}

Matrix Matrix::removePadding(int origRows, int origCols) const
{
    return submatrix(0, 0, origRows, origCols);
}

/* ============================================================================
 * STATIC UTILITY FUNCTIONS
 * ========================================================================= */

bool Matrix::isPowerOf2(int n)
{
    return n > 0 && (n & (n - 1)) == 0;
}

int Matrix::nextPowerOf2(int n)
{
    if (n <= 0)
        return 1;

    int power = 1;
    while (power < n)
    {
        power *= 2;
    }
    return power;
}



/* ============================================================================
 * NON-MEMBER FUNCTIONS
 * ========================================================================= */

Matrix operator*(double scalar, const Matrix &m)
{
    return m * scalar;
}

std::ostream &operator<<(std::ostream &os, const Matrix &m)
{
    m.print("Matrix");
    return os;
}