#include "matrix.h"
#include "test_cases.h"
#include <cassert>

void test_simple_strassen() {
    int m = 2, n = 1, p = 3;
    vector<double> A = {1, 2};

    vector<double> B = {
        1, 2, 3
    };

    vector<double> C = {
        1, 2, 3, 2, 4, 6
    };

    assert(strassen(A, B, m, n, p) == C);
}

int main() {
    test_simple_strassen();
    return 0;
}
