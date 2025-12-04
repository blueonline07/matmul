#include "matrix.h"

vector<double> multiply(const vector<double> &A, const vector<double> &B, int m, int n, int p)
{
    vector<double> C(m * p, 0.0);
    const int BS = 64;

    using simd_type = simd<double>;
    constexpr int simd_size = simd_type::size();

    for (int i0 = 0; i0 < m; i0 += BS)
        for (int k0 = 0; k0 < n; k0 += BS)
            for (int j0 = 0; j0 < p; j0 += BS)
                for (int i = i0; i < min(i0 + BS, m); ++i)
                    for (int k = k0; k < min(k0 + BS, n); ++k) {
                        simd_type aik(A[i * n + k]);
                        int j = j0;

                        for (; j + simd_size - 1 < min(j0 + BS, p); j += simd_size) {
                            simd_type cVec(&C[i * p + j], element_aligned);
                            simd_type bVec(&B[k * p + j], element_aligned);
                            cVec += aik * bVec;
                            cVec.copy_to(&C[i * p + j], element_aligned);
                        }

                        for (; j < min(j0 + BS, p); ++j)
                            C[i * p + j] += A[i * n + k] * B[k * p + j];
                    }

    return C;
}
