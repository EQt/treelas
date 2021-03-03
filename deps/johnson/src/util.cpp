#include "util.hpp"

#include <stdarg.h>
#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <R_ext/Print.h>

#include <vector>

using namespace std;

namespace Util {

void LogToFile(const char * fname, const char * str, int indent_amt) {
  static int indent = 0;

  FILE * fobj = fopen(fname, "a");
  if (fobj) {
    for (int i = 0; i < indent; ++i) {
      fputs(" ", fobj);
    }
    fputs (str, fobj);
    fclose (fobj);
  }
  indent += indent_amt;
}

void LogToFileSprintf(const char * fname, int indent_amt, const char * format,
                      ...) {
  static int indent = 0;

  char buffer[1024];
  FILE * fobj = fopen(fname, "a");
  if (fobj) {
    for (int i = 0; i < indent; ++i) {
      fputs(" ", fobj);
    }

    va_list args;
    va_start (args, format);
    vsprintf (buffer, format, args);
    va_end (args);

    fputs (buffer, fobj);

    fclose (fobj);
  }
  indent += indent_amt;
}

void GetMatrixDim(SEXP m, int * n_row, int * n_col) {
  SEXP dbDim;   PROTECT(dbDim = GET_DIM(m));
  if(n_col) *n_col = INTEGER(dbDim)[1];
  if(n_row) *n_row = INTEGER(dbDim)[0];

  UNPROTECT(1);
}

void SetMatrixDim(SEXP m, int n_row, int n_col) {
  SEXP mDim;   PROTECT(mDim = NEW_INTEGER(2));
  INTEGER(mDim)[0] = n_row;
  INTEGER(mDim)[1] = n_col;
  SET_DIM(m, mDim);
  UNPROTECT(1);
}

// Gets an int from an R object.
int GetInt(SEXP p, int default_val, int* err_code) {
  if (p == R_NilValue) {
    if(err_code) *err_code = 1;
    return default_val;

  } else if(IS_INTEGER(p)) {
    return INTEGER(p)[0];

  } else if(IS_LOGICAL(p)) {
    if(LOGICAL(p)[0]) return 1;
    else return 0;

  } else if(IS_NUMERIC(p)) {
    return (int)(REAL(p)[0]);

  } else {
    if(err_code) *err_code = 2;
    return default_val;
  }
}

// Gets an double from an R object.
double GetNumeric(SEXP p, double default_val, int* err_code) {
  if (p == R_NilValue) {
    if(err_code) *err_code = 1;
    return default_val;

  } else if(IS_INTEGER(p)) {
    return INTEGER(p)[0];

  } else if(IS_LOGICAL(p)) {
    if(LOGICAL(p)[0]) return 1.0;
    else return 0.0;

  } else if(IS_NUMERIC(p)) {
    return REAL(p)[0];

  } else{
    if(err_code) *err_code = 2;
    return default_val;
  }
}

SEXP ListEltByName(SEXP vec, const char * str, int * elt_idx) {
  SEXP names = getAttrib(vec, R_NamesSymbol);
  int n_elt = LENGTH(vec);

  for (int idx = 0; idx < n_elt; idx++) {
    const char * s = CHAR(STRING_ELT(names, idx));

    bool found_elt = true;
    for (int k = 0; ; ++k) {
      if (s[k] != str[k]) {
        found_elt = false;
      }
      if (s[k] == '\0' || str[k] == '\0') {
        break;
      }
    }
    if (found_elt) {
      if (elt_idx) *elt_idx = idx;
      return(VECTOR_ELT(vec, idx));
    }
  }
  if (elt_idx) *elt_idx = -1;
  return R_NilValue;
}

void SaveIntVecToSxp(SEXP vec, int idx, const IntVec& v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    int m = v.size();

    SEXP v_sxp;
    PROTECT(v_sxp = NEW_INTEGER(m));
    int * vi = INTEGER(v_sxp);
    for (int i = 0; i < m;  ++i) {
      vi[i] = v[i];
    }

    SET_VECTOR_ELT(vec, idx, v_sxp);
    UNPROTECT(1);
  } else {
    error("idx = %d\n", idx);
  }
}

void SaveDblVecToSxp(SEXP vec, int idx, const RealVec& v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    int m = v.size();

    SEXP v_sxp;
    PROTECT(v_sxp = NEW_NUMERIC(m));
    double * vi = REAL(v_sxp);
    for (int i = 0; i < m;  ++i) {
      vi[i] = v[i];
    }

    SET_VECTOR_ELT(vec, idx, v_sxp);
    UNPROTECT(1);
  }
}


bool SxpToIntVec(SEXP vec, IntVec* v) {
  if (vec == R_NilValue || !IS_INTEGER(vec)) {
    return false;
  }
  int m = LENGTH(vec);
  int * vi = INTEGER(vec);
  *v = IntVec(m);
  for (int i = 0; i < m;  ++i) {
    (*v)[i] = vi[i];
  }
  return true;
}

void SxpVecEltToIntVec(SEXP vec, int idx, IntVec* v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    SxpToIntVec(VECTOR_ELT(vec, idx), v);
  } else {
    error("idx = %d\n", idx);
  }
}

void SxpVecEltToIntVecs(SEXP vec, int idx, IntVec2* v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    SEXP v_sxp = VECTOR_ELT(vec, idx);
    int m = LENGTH(v_sxp);

    *v = IntVec2(m);
    for (int i = 0; i < m;  ++i) {
      SxpVecEltToIntVec(v_sxp, i, &((*v)[i]));
    }
  } else {
    error("idx = %d\n", idx);
  }
}

bool SxpToDblVec(SEXP vec, RealVec* v) {
  if (!IS_NUMERIC(vec)) {
    return false;
  }

  int m = LENGTH(vec);
  double * vi = REAL(vec);
  *v = RealVec(m);
  for (int i = 0; i < m;  ++i) {
    (*v)[i] = vi[i];
  }
  return true;
}

void SxpVecEltToDblVec(SEXP vec, int idx, RealVec* v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    SxpToDblVec(VECTOR_ELT(vec, idx), v);
  } else {
    error("idx = %d\n", idx);
  }
}

void SxpVecEltToDblVecs(SEXP vec, int idx, RealVec2* v) {
  if (idx >= 0 && idx < LENGTH(vec)) {
    SEXP v_sxp = VECTOR_ELT(vec, idx);
    int m = LENGTH(v_sxp);

    *v = RealVec2(m);
    for (int i = 0; i < m;  ++i) {
      SxpVecEltToDblVec(v_sxp, i, &((*v)[i]));
    }
  } else {
    error("idx = %d\n", idx);
  }
}

int ListEltIdxByName(SEXP v, const char * name) {
  int idx = -1;
  ListEltByName(v, name, &idx);
  return idx;
}

}
