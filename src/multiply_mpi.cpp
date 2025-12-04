#include "matrix.h"
#include <mpi.h>
Matrix Matrix::multiplyMPI(const Matrix &A, const Matrix &B, int size, int rank)
{
    
    return multiplyNaive(A, B);
}

Matrix Matrix::multiplyStrassenMPI(const Matrix &A, const Matrix &B, int size, int rank)
{
    return multiplyStrassen(A, B);
}
