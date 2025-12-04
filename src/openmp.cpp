#include "matrix.h"
#include "omp.h"

vector<double> multiply_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p)
{
    vector<double> C(m * p);
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < p; j++)
        {
            for (int k = 0; k < n; k++)
            {
                C[i * p + j] += A[i * n + k] * B[k * p + j];
            }
        }
    }
    return C;
}
