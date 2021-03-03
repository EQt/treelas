#pragma once
#include <R.h>
#include <Rdefines.h>
#include <vector>

namespace Util {

typedef std::vector<double> RealVec;

typedef std::vector<int> IntVec;

typedef std::vector<RealVec> RealVec2;

typedef std::vector<IntVec> IntVec2;


int
GetInt(SEXP p, int default_val, int *err_code);

double
GetNumeric(SEXP p, double default_val, int* err_code);

bool
SxpToDblVec(SEXP vec, RealVec *v);

template <class T>
void
AllocTwoVec(int dim1, int dim2, std::vector<std::vector<T>> *m)
{
    m->clear();
    m->resize(dim1);
    for (int i = 0; i < dim1; ++i) {
        (*m)[i].resize(dim2);
    }
}

} // namespace Util
