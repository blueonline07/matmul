#include "matrix.h"

vector<double> add(const vector<double> &A, const vector<double> &B, int size)
{
    vector<double> C(size * size);
    using simd_type = simd<double>;
    constexpr int simd_size = simd_type::size();
    int i = 0;
    for (; i + simd_size - 1 < size * size; i += simd_size)
    {
        simd_type aVec(&A[i], element_aligned);
        simd_type bVec(&B[i], element_aligned);
        simd_type cVec = aVec + bVec;
        cVec.copy_to(&C[i], element_aligned);
    }
    for (; i < size * size; ++i)
    {
        C[i] = A[i] + B[i];
    }
    return C;
}

vector<double> sub(const vector<double> &A, const vector<double> &B, int size)
{
    vector<double> C(size * size);
    using simd_type = simd<double>;
    constexpr int simd_size = simd_type::size();
    int i = 0;
    for (; i + simd_size - 1 < size * size; i += simd_size)
    {
        simd_type aVec(&A[i], element_aligned);
        simd_type bVec(&B[i], element_aligned);
        simd_type cVec = aVec - bVec;
        cVec.copy_to(&C[i], element_aligned);
    }
    for (; i < size * size; ++i)
    {
        C[i] = A[i] - B[i];
    }
    return C;
}