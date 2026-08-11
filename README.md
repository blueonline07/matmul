# matmul — Parallel Matrix Multiplication in C++23

![C++23](https://img.shields.io/badge/C%2B%2B-23-00599C?logo=cplusplus&logoColor=white)
![OpenMP](https://img.shields.io/badge/OpenMP-shared--memory-266DB6)
![MPI](https://img.shields.io/badge/MPI-distributed-4CAF50)
![Eigen](https://img.shields.io/badge/verified_against-Eigen3-8A2BE2)

Dense double-precision matrix multiplication built around a single cache-blocked,
SIMD-vectorized kernel, scaled across cores and nodes four different ways — plus a
Strassen variant of each. Every implementation is verified against Eigen.

## Implementations

Two algorithm families × four execution models:

|                                | Serial     | OpenMP         | MPI            | Hybrid (MPI + OpenMP) |
| ------------------------------ | ---------- | -------------- | -------------- | --------------------- |
| **Classical** (blocked + SIMD) | `multiply` | `multiply_omp` | `multiply_mpi` | `multiply_hybrid`     |
| **Strassen** (one-level split) | `strassen` | `strassen_omp` | `strassen_mpi` | `strassen_hybrid`     |

- **Classical kernel** — `i·k·j` loop order with 64×64×64 cache blocking
  (`BS = 64` in `include/matrix.h`); the innermost loop is vectorized with
  `std::experimental::simd`. The `i·k·j` order keeps `B` and `C` accesses
  unit-stride, so no transpose is needed.
- **OpenMP** — `#pragma omp parallel for` over the 64-row blocks of `C`.
  Threads own disjoint row blocks, so no synchronization is needed.
- **MPI** — rank 0 broadcasts `B` and scatters contiguous row blocks of `A`
  (row count padded so it divides evenly among ranks); each rank runs the serial
  kernel on its block; results are gathered back to rank 0.
- **Hybrid** — the same MPI row distribution, with the OpenMP kernel inside each rank.
- **Strassen** — one level of Strassen splitting: the seven half-size products
  M1…M7 are computed with the blocked kernel and recombined into the four
  quadrants of `C`. The serial and OpenMP variants fall back to the plain kernel
  for `N ≤ 1024` (`THRESHOLD`) or non-square shapes; above the threshold they
  require an **even** `N` (see [Notes](#notes--limitations)).

### Strassen over MPI

`strassen_mpi` distributes exactly one subproduct per rank using tagged
point-to-point messages — no collectives:

```
                     rank 0 (root)
      pad A, B to the next power of two,
      split into quadrants A11..A22, B11..B22
           │  MPI_Send (tagged quadrants)
           ├──▶ rank 1   M2 = (A21 + A22) · B11
           ├──▶ rank 2   M3 = A11 · (B12 − B22)
           ├──▶ rank 3   M4 = A22 · (B21 − B11)
           ├──▶ rank 4   M5 = (A11 + A12) · B22
           ├──▶ rank 5   M6 = (A21 − A11) · (B11 + B12)
           ├──▶ rank 6   M7 = (A12 − A22) · (B21 + B22)
           │
      rank 0 computes M1 = (A11 + A22) · (B11 + B22)
           │  MPI_Recv results from ranks 1–6
           ▼
      recombine C11..C22, strip the padding
```

It therefore **requires at least 7 MPI ranks** (extra ranks stay idle);
`strassen_mpi` refuses to start with fewer. `strassen_hybrid` uses the same
7-rank layout but computes each product with the OpenMP kernel, giving
7 ranks × `OMP_NUM_THREADS` threads of parallelism — note it does **not** pad
(it requires square, even `N`) and has no rank-count guard, so launching it
with fewer than 7 ranks hangs.

## Project layout

```
matmul/
├── include/
│   ├── matrix.h            # kernel declarations, BS / THRESHOLD constants, MPI tags
│   └── test_cases.h        # test-driver declarations
├── src/
│   ├── multiply.cpp        # serial cache-blocked + SIMD kernel
│   ├── multiply_openmp.cpp # + OpenMP parallel-for
│   ├── multiply_mpi.cpp    # + MPI scatter/gather row distribution
│   ├── multiply_hybrid.cpp # MPI ranks × OpenMP threads
│   ├── strassen.cpp        # one-level Strassen, serial products
│   ├── strassen_omp.cpp    # Strassen with OpenMP products
│   ├── strassen_mpi.cpp    # Strassen, one product per MPI rank (p2p)
│   ├── strassen_hybrid.cpp # Strassen, MPI ranks × OpenMP threads
│   └── utils.cpp           # SIMD add/sub, next_pow2
├── tests/                  # one driver per binary + Eigen reference check
├── Makefile
└── test.sh                 # benchmark sweep → results_N<size>.csv
```

## Requirements

- A C++23 compiler with `<experimental/simd>` — the Makefile uses **g++-15**
  (Apple Clang won't do; adjust `CXX_SERIAL` / `CXX_OMP` for other GCC versions)
- OpenMP
- An MPI implementation (`mpicxx` / `mpirun`)
- Eigen3 — headers only, used by the tests as the correctness reference

On macOS:

```bash
brew install gcc open-mpi eigen
export OMPI_CXX=g++-15    # make mpicxx wrap GCC instead of Apple Clang
```

The `OMPI_CXX` export matters: Homebrew's `mpicxx` invokes `clang++` by
default, which has no `<experimental/simd>`, so the MPI targets won't build
without it.

If Eigen lives somewhere other than `/opt/homebrew/include/eigen3`, point the
build at it:

```bash
export EIGEN=/path/to/eigen3
```

## Build

```bash
make all
```

builds five test binaries into `bin/`: `test_serial`, `test_omp`, `test_mpi`,
`test_hybrid`, and `test_strassen`.

## Run

Each binary multiplies two `N × N` matrices, checks the result against Eigen,
and prints the wall-clock seconds per implementation. `N` defaults to `1000`.

```bash
make test N=512                # run everything below in sequence

make test_serial   N=2000                       # multiply + strassen
make test_omp      N=2000 OMP_NUM_THREADS=8     # multiply_omp + strassen_omp
make test_mpi      N=2000 MPI_NUM_PROC=8        # multiply_mpi
make test_hybrid   N=2000 MPI_NUM_PROC=8 OMP_NUM_THREADS=8
make test_strassen N=2048                       # strassen_mpi + strassen_hybrid, always 7 ranks
```

Defaults: `OMP_NUM_THREADS=8`, `MPI_NUM_PROC=8`. `test_strassen` always
launches with `mpirun -np 7` (one rank per Strassen subproduct).

> [!NOTE]
> With the default `N=1000`, `strassen` and `strassen_omp` take the
> `N ≤ 1024` fallback and just run the classical kernel. Use an even
> `N > 1024` (e.g. `N=2048`) to actually exercise Strassen.

To run the MPI targets across machines, pass a host list:

```bash
make test_mpi N=4000 MPI_NUM_PROC=16 HOSTS=node1,node2
```

The `-hosts` flag the Makefile passes to `mpirun` is MPICH syntax. With
Open MPI, use `--host node1:8,node2:8` instead (and note Open MPI won't
oversubscribe: `-np 8` needs 8 available slots/cores).

## Benchmark

`test.sh` sweeps every implementation and writes a CSV with speedup and
efficiency relative to the serial classical kernel:

```bash
./test.sh N=2000     # → results_N2000.csv
```

It runs: serial, OpenMP at 8 and 4 threads, then MPI / hybrid / Strassen across
0–3 hosts. The host names (`MPI-node14`–`MPI-node16`) are cluster-specific —
edit `HOST_NODES` in `test.sh` for your environment. On a single machine only
the 0-host pass produces rows; the 1–3-host passes fail and their rows are
skipped.

```csv
method,threads,procs,hosts,time,speedup,efficiency
serial_naive,1,1,"",4.212389,1.0,1.0
omp_naive,8,1,"",0.612345,6.878965,0.859871
...
```

Speedup is always `serial_naive time ÷ row time`. The efficiency denominator
varies by row: thread count for OpenMP and hybrid rows, process count for MPI
rows (7 for `mpi_strassen`) — so efficiency is comparable within a method, not
across methods.

## Correctness

Every driver fills `A` and `B` with ones and asserts the result equals Eigen's,
element for element. With all-ones inputs each output element is exactly `N`
(an integer, exactly representable in a double), so the comparison is exact
despite floating-point arithmetic and `-ffast-math`.

## Notes & limitations

- **One-level Strassen** — the Strassen variants split once and hand the seven
  half-size products to the blocked kernel; they do not recurse further.
- **Shapes** — the classical kernels accept arbitrary `m × n · n × p`.
  `strassen`/`strassen_omp` fall back to the classical kernel for non-square
  inputs, but an **odd** square `N > 1024` is not caught: the `N/2` split
  truncates and the last row/column of the result is silently wrong.
  `strassen_mpi` pads everything to the next power of two and has no such
  restriction; `strassen_hybrid` assumes square matrices with even `N`.
- **Compiled hot** — `-O3 -ffast-math -funroll-loops -march=native`: binaries
  are tuned to the build machine and arithmetic is not strictly IEEE-compliant.
- **`std::experimental::simd`** — a GCC/libstdc++ TS header (the precursor of
  C++26 `std::simd`); this is why the build pins `g++-15`.
