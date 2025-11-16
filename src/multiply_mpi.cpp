#include "matrix.h"
#include <mpi.h>

Matrix Matrix::multiplyMPI(const Matrix &A, const Matrix &B)
{
    // TODO: Implement MPI version
    // MPI is initialized/finalized in main program
    // This function just does the computation
    return multiplyNaive(A, B);
}

Matrix Matrix::multiplyHybrid(const Matrix &A, const Matrix &B)
{
    // TODO: Implement Hybrid MPI+OpenMP
    // MPI is initialized/finalized in main program
    // This function just does the computation
    return multiplyNaive(A, B);
}
