#pragma once
#include <vector>
#include <cstdint>
#include <cstdio>

extern "C" {

struct CSCMat {
    int64_t  m;         // number of rows
    int64_t  n;         // number of cols
    const int64_t *colptr;
    const int64_t *rowval;
    const double  *nzval;
};


/** Transpose(matrix) times vector: Compute x := A' * v */
void
T_mul_b(double *x,
        const CSCMat *A,
        const double *v);

/** Matrix times vector: Compute x := A * v */
void
A_mul_b(double *x,
        const CSCMat *A,
        const double *v);

}   //  extern "C"


/** Create a new sparse matrix (I, J, and W don't need to be sorted). */
CSCMat
sparse(std::vector<int64_t> I,
       std::vector<int64_t> J,
       std::vector<double>  W,
       int64_t m, int64_t n);

int64_t
num_nonzeros(const CSCMat *mat);

void
print_CSCMat(const CSCMat *mat, FILE *out = stdout);
