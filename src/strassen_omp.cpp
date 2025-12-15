#include "matrix.h"
#include <omp.h>

vector<double> strassen_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p)
{
    if (m <= THRESHOLD || m != n || n != p)
    {
        return multiply_omp(A, B, m, n, p);
    }

    int h = m / 2;
    int hs = h * h;

    vector<double> A11(hs), A12(hs), A21(hs), A22(hs);
    vector<double> B11(hs), B12(hs), B21(hs), B22(hs);

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < h; j++)
        {
            A11[i * h + j] = A[i * m + j];
            A12[i * h + j] = A[i * m + j + h];
            A21[i * h + j] = A[(i + h) * m + j];
            A22[i * h + j] = A[(i + h) * m + j + h];

            B11[i * h + j] = B[i * m + j];
            B12[i * h + j] = B[i * m + j + h];
            B21[i * h + j] = B[(i + h) * m + j];
            B22[i * h + j] = B[(i + h) * m + j + h];
        }
    }
    vector<double> op1, op2;
    vector<double> M1, M2, M3, M4, M5, M6, M7;
    // M1 = (A11 + A22) * (B11 + B22)
    add(A11, A22, op1, h);
    add(B11, B22, op2, h);
    M1 = multiply_omp(op1, op2, h, h, h);

    // M2 = (A21 + A22) * B11
    add(A21, A22, op1, h);
    M2 = multiply_omp(op1, B11, h, h, h);

    // M3 = A11 * (B12 - B22)
    sub(B12, B22, op2, h);
    M3 = multiply_omp(A11, op2, h, h, h);

    // M4 = A22 * (B21 - B11)
    sub(B21, B11, op2, h);
    M4 = multiply_omp(A22, op2, h, h, h);

    // M5 = (A11 + A12) * B22
    add(A11, A12, op1, h);
    M5 = multiply_omp(op1, B22, h, h, h);

    // M6 = (A21 - A11) * (B11 + B12)
    sub(A21, A11, op1, h);
    add(B11, B12, op2, h);
    M6 = multiply_omp(op1, op2, h, h, h);

    // M7 = (A12 - A22) * (B21 + B22)
    sub(A12, A22, op1, h);
    add(B21, B22, op2, h);
    M7 = multiply_omp(op1, op2, h, h, h);

    vector<double> C11(hs), C12(hs), C21(hs), C22(hs);

    // C11 = M1 + M4 - M5 + M7
    add(M1, M4, op1, h);
    sub(op1, M5, op1, h);
    add(op1, M7, C11, h);

    // C12 = M3 + M5
    add(M3, M5, C12, h);

    // C21 = M2 + M4
    add(M2, M4, C21, h);

    // C22 = M1 + M3 - M2 + M6
    add(M1, M3, op1, h);
    sub(op1, M2, op1, h);
    add(op1, M6, C22, h);

    vector<double> C(m * m);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < h; j++)
        {
            C[i * m + j] = C11[i * h + j];
            C[i * m + j + h] = C12[i * h + j];
            C[(i + h) * m + j] = C21[i * h + j];
            C[(i + h) * m + j + h] = C22[i * h + j];
        }
    }

    return C;
}