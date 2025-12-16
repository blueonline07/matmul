#include "matrix.h"
#include <chrono>
#include <cassert>

vector<double> libcheck(const vector<double> &, const vector<double> &, int, int, int);

void test_strassen_hybrid(int, int, int);
void test_strassen_mpi(int, int, int);
void test_strassen_omp(int);
void test_strassen(int);
void test_hybrid(int, int, int);
void test_mpi(int, int, int);
void test_omp(int);
void test_serial(int);