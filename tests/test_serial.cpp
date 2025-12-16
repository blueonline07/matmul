#include "matrix.h"
#include "test_cases.h"

int main(int argc, char *argv[])
{
    int N = 1000;
    if (argc > 1)
    {
        N = atoi(argv[1]);
    }
    test_serial(N);
    test_strassen(N);
    return 0;
}

void test_serial(int N)
{
    int m = N, n = N, p = N;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++)
    {
        A[i] = 1;
    }

    for (int i = 0; i < n * p; i++)
    {
        B[i] = 1;
    }

    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = multiply(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
    t0 = chrono::high_resolution_clock::now();
    assert(C == libcheck(A, B, m, n, p));
    t1 = chrono::high_resolution_clock::now();
    cout << "eigen: " << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
}

void test_strassen(int N)
{
    int m = N, n = N, p = N;
    vector<double> A(m * n);
    vector<double> B(n * p);

    for (int i = 0; i < m * n; i++)
    {
        A[i] = 1;
    }

    for (int i = 0; i < n * p; i++)
    {
        B[i] = 1;
    }

    auto t0 = chrono::high_resolution_clock::now();
    vector<double> C = strassen(A, B, m, n, p);
    auto t1 = chrono::high_resolution_clock::now();

    cout << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
    t0 = chrono::high_resolution_clock::now();
    assert(C == libcheck(A, B, m, n, p));
    t1 = chrono::high_resolution_clock::now();
    cout << "eigen: " << chrono::duration_cast<chrono::duration<double>>(t1 - t0).count() << endl;
}