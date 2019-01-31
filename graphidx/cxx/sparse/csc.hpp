#pragma once
#include <vector>
#include <cstdint>
#include <cstdio>

/** Compressed Sparse Column Matrix */
struct CSCMat {
    int64_t  m;         // number of rows
    int64_t  n;         // number of cols
    const int64_t *colptr;
    const int64_t *rowval;
    const double  *nzval;
    // const bool own_data = false;
    // ~CSCMat();
};


/** Transpose(matrix) times vector: Compute x := A' * v */
extern "C" void
T_mul_b(double *x,
        const CSCMat *A,
        const double *v);

/** Matrix times vector: Compute x := A * v */
extern "C" void
A_mul_b(double *x,
        const CSCMat *A,
        const double *v);


/** Create a new sparse matrix (I, J, and W don't need to be sorted). */
CSCMat
sparse(std::vector<int64_t> I,
       std::vector<int64_t> J,
       std::vector<double>  W,
       int64_t m, int64_t n);


/** Number of non-zero entry in a matrix */
int64_t
num_nonzeros(const CSCMat *mat);


/** Print matrix to `out` similar to `show(::SparseMatrixCSC)` in Julia. */
void
print_CSCMat(const CSCMat *mat, FILE *out = stdout);
