#include "matrix.h"
#include "test_cases.h"
#include <Eigen/Dense>

vector<double> libcheck(const vector<double> &A, const vector<double> &B, int m, int n, int p){
    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        A_eig(A.data(), m, n);

    Eigen::Map<const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        B_eig(B.data(), n, p);
    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
        C_eig = A_eig * B_eig;
    
    vector<double> C(m * p);
    memcpy(C.data(), C_eig.data(), sizeof(double) * m * p);
    return C;
}