#include "matrix.h"
#include <chrono>
#include <cassert>

vector<double> libcheck(const vector<double> &, const vector<double> &, int , int, int);

void test_serial_simple();
void test_omp_simple();
void test_strassen_simple();
void test_mpi_simple(int, int);

void test_strassen_hybrid_large(int, int, int);
void test_strassen_mpi_large(int, int, int);
void test_strassen_omp_large(int);
void test_strassen_large(int);
void test_hybrid_large(int, int, int);
void test_mpi_large(int, int, int);
void test_omp_large(int);
void test_serial_large(int);

void test_serial_zeros();
void test_serial_identity();
void test_omp_zeros();
void test_omp_identity();
void test_mpi_zeros(int, int);
void test_mpi_identity(int, int);
void test_hybrid_zeros(int, int);
void test_hybrid_identity(int, int);