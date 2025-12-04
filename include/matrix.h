#include <vector>
#include <iostream>
#include <chrono>

using namespace std;

vector<double> add(const vector<double> &A, const vector<double> &B, int size);
vector<double> sub(const vector<double> &A, const vector<double> &B, int size);


// A: m * n
// B: n * p
vector<double> multiply(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> multiply_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> strassen(const vector<double> &A, const vector<double> &B, int m, int n, int p);
vector<double> strassen_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p);