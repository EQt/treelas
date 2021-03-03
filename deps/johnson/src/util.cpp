#include "util.hpp"

namespace Util {
// Functions for converting from R structures to C/C++ data structures
// Gets an int from an R object.
int
GetInt(SEXP p, int default_val, int *err_code)
{
    if (p == R_NilValue) {
        if (err_code)
            *err_code = 1;
        return default_val;

    } else if (IS_INTEGER(p)) {
        return INTEGER(p)[0];

    } else if (IS_LOGICAL(p)) {
        if (LOGICAL(p)[0])
            return 1;
        else
            return 0;

    } else if (IS_NUMERIC(p)) {
        return (int)(REAL(p)[0]);

    } else {
        if (err_code)
            *err_code = 2;
        return default_val;
    }
}

// void
// SxpVecEltToDblVec(SEXP vec, int idx, RealVec *v)
// {
//     if (idx >= 0 && idx < LENGTH(vec)) {
//         SxpToDblVec(VECTOR_ELT(vec, idx), v);
//     } else {
//         error("idx = %d\n", idx);
//     }
// }

bool
SxpToDblVec(SEXP vec, RealVec *v)
{
    if (!IS_NUMERIC(vec)) {
        return false;
    }

    int m = LENGTH(vec);
    double *vi = REAL(vec);
    *v = RealVec(m);
    for (int i = 0; i < m; ++i) {
        (*v)[i] = vi[i];
    }
    return true;
}

// Gets an double from an R object.
double
GetNumeric(SEXP p, double default_val, int *err_code)
{
    if (p == R_NilValue) {
        if (err_code)
            *err_code = 1;
        return default_val;

    } else if (IS_INTEGER(p)) {
        return INTEGER(p)[0];

    } else if (IS_LOGICAL(p)) {
        if (LOGICAL(p)[0])
            return 1.0;
        else
            return 0.0;

    } else if (IS_NUMERIC(p)) {
        return REAL(p)[0];

    } else {
        if (err_code)
            *err_code = 2;
        return default_val;
    }
}

} // namespace Util
