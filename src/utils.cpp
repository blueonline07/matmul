#include "matrix.h"

vector<double> add(const vector<double> &A, const vector<double> &B, int size){
    vector<double> C(size * size);
    for (int i = 0; i < size * size; i++) {
        C[i] = A[i] + B[i];
    }
    return C;
}

vector<double> sub(const vector<double> &A, const vector<double> &B, int size){
    vector<double> C(size * size);
    for (int i = 0; i < size * size; i++) {
        C[i] = A[i] - B[i];
    }
    return C;
}
