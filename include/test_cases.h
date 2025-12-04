#include "matrix.h"
#include <Eigen/Dense>
#include <chrono>
#include <cassert>

void test_simple_serial();
void test_simple_omp();
void test_omp_large(int);
void test_simple_strassen();
void test_serial_zeros();
void test_serial_identity();
void test_serial_large(int);