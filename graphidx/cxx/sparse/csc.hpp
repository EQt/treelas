#pragma once
#include <vector>
#include <cstdint>
#include <cstdio>

/** Compressed Sparse Column Matrix */
struct CSCMat
{
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
template <typename Idx = int64_t,
          typename Val = double>
CSCMat
sparse(std::vector<Idx> I,
       std::vector<Idx> J,
       std::vector<Val> W,
       const size_t m,
       const size_t n)
{
    const auto nnz = I.size();
    if (J.size() != nnz)
        throw "J.size() is wrong";
    if (W.size() != nnz)
        throw "W.size() is wrong";

    std::vector<Idx> order (nnz);
    for (size_t i = 0; i < nnz; i++)
        order[i] = Idx(i);

    std::sort(order.begin(), order.end(), [&](Idx i, Idx j)
        { return (J[i] < J[j]) || (J[i] == J[j] && I[i] < I[j]); });

    // for (auto o : order) {
    //     std::cerr << " " << o;
    // }
    // std::cerr << std::endl;

    // for (auto o : order) {
    //     std::cerr << " " << I[o];
    // }
    // std::cerr << std::endl;

    // for (auto o : order) {
    //     std::cerr << " " << J[o];
    // }
    // std::cerr << std::endl;

    // for (auto o : order) {
    //     std::cerr << " " << W[o];
    // }
    // std::cerr << std::endl;

    Val  *nzval = new Val[nnz];
    Idx *rowval = new Idx[nnz];
    Idx *colptr = new Idx[n+1];
    colptr[n] = nnz+1;
    colptr[0] = 1;

    Idx k = 1;
    for (size_t i = 0; i < nnz; i++) {
        while (J[order[i]] > k)
            colptr[k++] = i+1;

        rowval[i] = I[order[i]];
        nzval[i] = W[order[i]];
    }

    return CSCMat({m, n, colptr, rowval, nzval});
}


/** Number of non-zero entry in a matrix */
int64_t
num_nonzeros(const CSCMat *mat);


/** Print matrix to `out` similar to `show(::SparseMatrixCSC)` in Julia. */
void
print_CSCMat(const CSCMat *mat, FILE *out = stdout);
