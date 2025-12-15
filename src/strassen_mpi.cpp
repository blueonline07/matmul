#include "matrix.h"
#include <mpi.h>

vector<double> strassen_mpi(const vector<double> &A, const vector<double> &B, int m, int n, int p, int rank, int size)
{
    int workers = min(size, 7);
    if (rank >= workers)
    {
        return vector<double>();
    }

    int h = m / 2;
    int hs = h * h;
    vector<double> A11, A12, A21, A22;
    vector<double> B11, B12, B21, B22;
    if (rank == 0)
    {
        A11.resize(hs);
        A12.resize(hs);
        A21.resize(hs);
        A22.resize(hs);
        B11.resize(hs);
        B12.resize(hs);
        B21.resize(hs);
        B22.resize(hs);

        for (int i = 0; i < h; i++)
        {
            for (int j = 0; j < h; j++)
            {
                A11[i * h + j] = A[i * m + j];
                A12[i * h + j] = A[i * m + j + h];
                A21[i * h + j] = A[(i + h) * m + j];
                A22[i * h + j] = A[(i + h) * m + j + h];

                B11[i * h + j] = B[i * n + j];
                B12[i * h + j] = B[i * n + j + h];
                B21[i * h + j] = B[(i + h) * n + j];
                B22[i * h + j] = B[(i + h) * n + j + h];
            }
        }
    }

    vector<double> local_M(hs, 0.0);

    if (rank == 0)
    {
        MPI_Send(A21.data(), hs, MPI_DOUBLE, 1, TAG_A21, MPI_COMM_WORLD);
        MPI_Send(A22.data(), hs, MPI_DOUBLE, 1, TAG_A22, MPI_COMM_WORLD);
        MPI_Send(B11.data(), hs, MPI_DOUBLE, 1, TAG_B11, MPI_COMM_WORLD);

        MPI_Send(A11.data(), hs, MPI_DOUBLE, 2, TAG_A11, MPI_COMM_WORLD);
        MPI_Send(B12.data(), hs, MPI_DOUBLE, 2, TAG_B12, MPI_COMM_WORLD);
        MPI_Send(B22.data(), hs, MPI_DOUBLE, 2, TAG_B22, MPI_COMM_WORLD);

        MPI_Send(A22.data(), hs, MPI_DOUBLE, 3, TAG_A22, MPI_COMM_WORLD);
        MPI_Send(B21.data(), hs, MPI_DOUBLE, 3, TAG_B21, MPI_COMM_WORLD);
        MPI_Send(B11.data(), hs, MPI_DOUBLE, 3, TAG_B11, MPI_COMM_WORLD);

        MPI_Send(A11.data(), hs, MPI_DOUBLE, 4, TAG_A11, MPI_COMM_WORLD);
        MPI_Send(A12.data(), hs, MPI_DOUBLE, 4, TAG_A12, MPI_COMM_WORLD);
        MPI_Send(B22.data(), hs, MPI_DOUBLE, 4, TAG_B22, MPI_COMM_WORLD);

        MPI_Send(A21.data(), hs, MPI_DOUBLE, 5, TAG_A21, MPI_COMM_WORLD);
        MPI_Send(A11.data(), hs, MPI_DOUBLE, 5, TAG_A11, MPI_COMM_WORLD);
        MPI_Send(B11.data(), hs, MPI_DOUBLE, 5, TAG_B11, MPI_COMM_WORLD);
        MPI_Send(B12.data(), hs, MPI_DOUBLE, 5, TAG_B12, MPI_COMM_WORLD);

        MPI_Send(A12.data(), hs, MPI_DOUBLE, 6, TAG_A12, MPI_COMM_WORLD);
        MPI_Send(A22.data(), hs, MPI_DOUBLE, 6, TAG_A22, MPI_COMM_WORLD);
        MPI_Send(B21.data(), hs, MPI_DOUBLE, 6, TAG_B21, MPI_COMM_WORLD);
        MPI_Send(B22.data(), hs, MPI_DOUBLE, 6, TAG_B22, MPI_COMM_WORLD);

        local_M = multiply(add(A11, A22, h), add(B11, B22, h), h, h, h);
    }

    else if (rank == 1)
    {
        // M2 = (A21 + A22) * B11
        A21.resize(hs);
        A22.resize(hs);
        B11.resize(hs);
        MPI_Recv(A21.data(), hs, MPI_DOUBLE, 0, TAG_A21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(A22.data(), hs, MPI_DOUBLE, 0, TAG_A22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B11.data(), hs, MPI_DOUBLE, 0, TAG_B11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_M = multiply(add(A21, A22, h), B11, h, h, h);
    }
    else if (rank == 2)
    {
        A11.resize(hs);
        B12.resize(hs);
        B22.resize(hs);

        MPI_Recv(A11.data(), hs, MPI_DOUBLE, 0, TAG_A11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B12.data(), hs, MPI_DOUBLE, 0, TAG_B12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B22.data(), hs, MPI_DOUBLE, 0, TAG_B22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_M = multiply(A11, sub(B12, B22, h), h, h, h);
    }
    else if (rank == 3)
    {
        A22.resize(hs);
        B21.resize(hs);
        B11.resize(hs);

        MPI_Recv(A22.data(), hs, MPI_DOUBLE, 0, TAG_A22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B21.data(), hs, MPI_DOUBLE, 0, TAG_B21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B11.data(), hs, MPI_DOUBLE, 0, TAG_B11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_M = multiply(A22, sub(B21, B11, h), h, h, h);
    }
    else if (rank == 4)
    {
        A11.resize(hs);
        A12.resize(hs);
        B22.resize(hs);

        MPI_Recv(A11.data(), hs, MPI_DOUBLE, 0, TAG_A11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(A12.data(), hs, MPI_DOUBLE, 0, TAG_A12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B22.data(), hs, MPI_DOUBLE, 0, TAG_B22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_M = multiply(add(A11, A12, h), B22, h, h, h);
    }
    else if (rank == 5)
    {
        A21.resize(hs);
        A11.resize(hs);
        B11.resize(hs);
        B12.resize(hs);

        MPI_Recv(A21.data(), hs, MPI_DOUBLE, 0, TAG_A21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(A11.data(), hs, MPI_DOUBLE, 0, TAG_A11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B11.data(), hs, MPI_DOUBLE, 0, TAG_B11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B12.data(), hs, MPI_DOUBLE, 0, TAG_B12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_M = multiply(sub(A21, A11, h), add(B11, B12, h), h, h, h);
    }
    else if (rank == 6)
    {
        A12.resize(hs);
        A22.resize(hs);
        B21.resize(hs);
        B22.resize(hs);

        MPI_Recv(A12.data(), hs, MPI_DOUBLE, 0, TAG_A12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(A22.data(), hs, MPI_DOUBLE, 0, TAG_A22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B21.data(), hs, MPI_DOUBLE, 0, TAG_B21, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(B22.data(), hs, MPI_DOUBLE, 0, TAG_B22, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        local_M = multiply(sub(A12, A22, h), add(B21, B22, h), h, h, h);
    }

    vector<double> M1(hs), M2(hs), M3(hs), M4(hs), M5(hs), M6(hs), M7(hs);

    if (size >= 7)
    {
        if (rank == 0)
        {
            M1 = local_M;
            MPI_Recv(M2.data(), hs, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M3.data(), hs, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M4.data(), hs, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M5.data(), hs, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M6.data(), hs, MPI_DOUBLE, 5, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(M7.data(), hs, MPI_DOUBLE, 6, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        else if (rank >= 1 && rank <= 6)
        {
            MPI_Send(local_M.data(), hs, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        if (rank == 0)
        {
            M1 = multiply(add(A11, A22, h), add(B11, B22, h), h, h, h);
            M2 = multiply(add(A21, A22, h), B11, h, h, h);
            M3 = multiply(A11, sub(B12, B22, h), h, h, h);
            M4 = multiply(A22, sub(B21, B11, h), h, h, h);
            M5 = multiply(add(A11, A12, h), B22, h, h, h);
            M6 = multiply(sub(A21, A11, h), add(B11, B12, h), h, h, h);
            M7 = multiply(sub(A12, A22, h), add(B21, B22, h), h, h, h);
        }
    }

    vector<double> C(m * m);

    if (rank == 0)
    {
        // C11 = M1 + M4 - M5 + M7
        auto C11 = add(sub(add(M1, M4, h), M5, h), M7, h);
        // C12 = M3 + M5
        auto C12 = add(M3, M5, h);
        // C21 = M2 + M4
        auto C21 = add(M2, M4, h);
        // C22 = M1 + M3 - M2 + M6
        auto C22 = add(sub(add(M1, M3, h), M2, h), M6, h);

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
    }

    return C;
}