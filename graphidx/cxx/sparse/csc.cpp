#include <algorithm>
#include <iostream>
#include "csc.hpp"

#ifdef __cplusplus
extern "C" {
#endif


void
_A_mul_b(double *x,
         const int64_t  m,
         const int64_t  n,
         const int64_t *colptr,
         const int64_t *rowval,
         const double  *nzval,
         const double  *v)
{
    #pragma omp parallel for simd
    for (int i = 0; i < m; i++)
        x[i] = 0;

    /* paralilization may change the solution due to race conditions */
    // #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        // #pragma omp simd reduction (+:x)
        for (int j = colptr[i]-1; j < colptr[i+1]-1; j++) {
            int k = rowval[j]-1;
            x[k] += nzval[j] * v[i];
        }
    }
}


void
A_mul_b(double *x,
        const CSCMat *A,
        const double *v)
{
    return _A_mul_b(x, A->m, A->n, A->colptr, A->rowval, A->nzval, v);
}


void
T_mul_b_(double *x,
         const int64_t  m,
         const int64_t  n,
         const int64_t *colptr,
         const int64_t *rowval,
         const double  *nzval,
         const double  *v)
{
    #pragma omp parallel for simd
    for (int i = 0; i < n; i++)
        x[i] = 0;

    /* paralilization may change the solution due to race conditions */
    // #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        // #pragma omp simd reduction (+:x)
        for (int j = colptr[i]-1; j < colptr[i+1]-1; j++) {
            int k = rowval[j]-1;
            x[i] += nzval[j] * v[k];
        }
    }
}

/** compute x = A * v */
void
T_mul_b(double *x,
        const CSCMat *A,
        const double *v)
{
    return T_mul_b_(x, A->m, A->n, A->colptr, A->rowval, A->nzval, v);
}


#ifdef __cplusplus
}   // extern "C"
#endif


CSCMat
sparse(std::vector<int64_t> I,
       std::vector<int64_t> J,
       std::vector<double>  W,
       int64_t m, int64_t n)
{
    const auto nnz = I.size();
    if (J.size() != nnz)
        throw "J.size() is wrong";
    if (W.size() != nnz)
        throw "W.size() is wrong";

    std::vector<int64_t> order (nnz);
    for (int i = 0; i < int(nnz); i++)
        order[i] = i;

    std::sort(order.begin(), order.end(), [&](int64_t i, int64_t j)
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

    double  *nzval = new double[nnz];
    int64_t *rowval = new int64_t[nnz];
    int64_t *colptr = new int64_t[n+1];
    colptr[n] = nnz+1;
    colptr[0] = 1;

    int k = 1;
    for (int i = 0; i < int(nnz); i++) {
        while (J[order[i]] > k) {
            colptr[k++] = i+1;
        }
        rowval[i] = I[order[i]];
        nzval[i] = W[order[i]];
    }

    return CSCMat({m, n, colptr, rowval, nzval});
}


int64_t
num_nonzeros(const CSCMat *mat)
{
    return mat->colptr[mat->n] - 1;
}


void
print_CSCMat(const CSCMat *mat, FILE *out)
{
    fprintf(out, "%ld×%ld matrix with %ld nnz:\n",
            long(mat->m), long(mat->n), long(num_nonzeros(mat)));
    for (int i = 0; i < mat->n; i++) {
        for (int j = mat->colptr[i]-1; j < mat->colptr[i+1]-1; j++) {
            int k = mat->rowval[j]-1;
            fprintf(out, "[%d, %d]  =  %f\n", k+1, i+1, mat->nzval[j]);
        }
    }
}
