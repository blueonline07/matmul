#include "matrix.h"

vector<double> strassen(const vector<double> &A, const vector<double> &B, int m, int n, int p)
{
    if (m <= THRESHOLD || m != n || n != p) {
        return multiply(A, B, m, n, p);
    }
    
    int h = m / 2;
    int hs = h * h;
    
    vector<double> A11(hs), A12(hs), A21(hs), A22(hs);
    vector<double> B11(hs), B12(hs), B21(hs), B22(hs);
    
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < h; j++) {
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
    
    auto M1 = strassen(add(A11, A22, h), add(B11, B22, h), h, h, h);
    auto M2 = strassen(add(A21, A22, h), B11, h, h, h);
    auto M3 = strassen(A11, sub(B12, B22, h), h, h, h);
    auto M4 = strassen(A22, sub(B21, B11, h), h, h, h);
    auto M5 = strassen(add(A11, A12, h), B22, h, h, h);
    auto M6 = strassen(sub(A21, A11, h), add(B11, B12, h), h, h, h);
    auto M7 = strassen(sub(A12, A22, h), add(B21, B22, h), h, h, h);
    
    auto C11 = add(sub(add(M1, M4, h), M5, h), M7, h);
    auto C12 = add(M3, M5, h);
    auto C21 = add(M2, M4, h);
    auto C22 = add(sub(add(M1, M3, h), M2, h), M6, h);
    
    vector<double> C(m * m);
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < h; j++) {
            C[i * m + j] = C11[i * h + j];
            C[i * m + j + h] = C12[i * h + j];
            C[(i + h) * m + j] = C21[i * h + j];
            C[(i + h) * m + j + h] = C22[i * h + j];
        }
    }
    
    return C;
}