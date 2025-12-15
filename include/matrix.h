#include <vector>
#include <iostream>
#include <chrono>
#include <experimental/simd>
#define BS 64
#define THRESHOLD 1024

using namespace std;
using namespace std::experimental::parallelism_v2;

#ifndef MPI_TAG_H
#define MPI_TAG_H
enum MPITag
{
    TAG_A11 = 1,
    TAG_A12 = 2,
    TAG_A21 = 3,
    TAG_A22 = 4,
    TAG_B11 = 5,
    TAG_B12 = 6,
    TAG_B21 = 7,
    TAG_B22 = 8,
    TAG_RESULT = 100
};
#endif

vector<double> add(const vector<double> &A, const vector<double> &B, int size);
vector<double> sub(const vector<double> &A, const vector<double> &B, int size);

// A: m * n
// B: n * p
vector<double> multiply(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> multiply_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> strassen(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> strassen_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p);

vector<double> multiply_mpi(vector<double> &A, vector<double> &B, int m, int n, int p, int rank, int size);
vector<double> strassen_mpi(const vector<double> &A, const vector<double> &B, int m, int n, int p, int rank, int size);

vector<double> multiply_hybrid(vector<double> &A, vector<double> &B, int m, int n, int p, int rank, int size);
vector<double> strassen_hybrid(const vector<double> &A, const vector<double> &B, int m, int n, int p, int rank, int size);