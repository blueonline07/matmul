#include "matrix.h"
#include <omp.h>

vector<double> strassen_omp(const vector<double> &A, const vector<double> &B, int m, int n, int p)
{

    if (m <= 64 || m != n || n != p) {
        return multiply(A, B, m, n, p);
    }
    
    int h = m / 2;
    int hs = h * h;
    
    vector<double> A11(hs), A12(hs), A21(hs), A22(hs);
    vector<double> B11(hs), B12(hs), B21(hs), B22(hs);
    
    #pragma omp parallel for collapse(2)
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
    
    // Compute 7 products in parallel
    vector<double> M1, M2, M3, M4, M5, M6, M7;
    
    #pragma omp parallel sections
    {
        #pragma omp section
        M1 = strassen_omp(add(A11, A22, h), add(B11, B22, h), h, h, h);
        
        #pragma omp section
        M2 = strassen_omp(add(A21, A22, h), B11, h, h, h);
        
        #pragma omp section
        M3 = strassen_omp(A11, sub(B12, B22, h), h, h, h);
        
        #pragma omp section
        M4 = strassen_omp(A22, sub(B21, B11, h), h, h, h);
        
        #pragma omp section
        M5 = strassen_omp(add(A11, A12, h), B22, h, h, h);
        
        #pragma omp section
        M6 = strassen_omp(sub(A21, A11, h), add(B11, B12, h), h, h, h);
        
        #pragma omp section
        M7 = strassen_omp(sub(A12, A22, h), add(B21, B22, h), h, h, h);
    }
    
    auto C11 = add(sub(add(M1, M4, h), M5, h), M7, h);
    auto C12 = add(M3, M5, h);
    auto C21 = add(M2, M4, h);
    auto C22 = add(sub(add(M1, M3, h), M2, h), M6, h);
    
    vector<double> C(m * m);
    #pragma omp parallel for collapse(2)
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