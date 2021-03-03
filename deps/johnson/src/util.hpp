#ifndef UTIL_HPP__
#define UTIL_HPP__

#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <R_ext/Print.h>

#include <vector>

using namespace std;

namespace Util {

void LogToFile(const char * fname, const char * str, int indent_amt = 0);
void LogToFileSprintf(const char * fname, int indent_amt, const char * format, ...);

__inline void ZeroBit(int idx, int * x) {
  *x &= ~(1 << idx);
}

template<class T> class PointerDeleter {
public:
  PointerDeleter(T * ptr) : ptr_(ptr) { }
  ~PointerDeleter() {
    if (ptr_) {
      delete ptr_;
    }
  }
private:
  T * ptr_;
};

template<class T> class VectorEltDeleter {
public:
	VectorEltDeleter(vector<T *>& v) : v_(v) { }
  ~VectorEltDeleter() {
  	typename vector<T *>::iterator v_iter;
  	for (v_iter = v_.begin(); v_iter != v_.end(); ++v_iter) {
  		if (*v_iter) {
  			delete *v_iter;
  			*v_iter = NULL;
  		}
  	}
  }
private:
  vector<T *>& v_;
};



template<class T> void RemoveRange(int lwr, int upr, vector<T> * seq) {
  vector<T> v;
  for (int i = 0, n = seq->size(); i < n; ++i) {
    if (i < lwr || i >= upr) {
      v.push_back((*seq)[i]);
    }
  }
  seq->swap(v);
}

template<class T> void ExtractRange(int lwr, int upr, vector<T> * seq) {
  vector<T> v;
  for (int i = 0, n = seq->size(); i < n; ++i) {
    if (i >= lwr && i < upr) {
      v.push_back((*seq)[i]);
    }
  }
  seq->swap(v);
}

template<class T> void PushVector(const vector<T>& v, vector<T> * seq) {
  for (int i = 0, n = v.size(); i < n; ++i) {
    seq->push_back(v[i]);
  }
}

typedef pair<double, double> RealPair;
typedef pair<int, int> IntPair;
typedef vector<double> RealVec;
typedef vector<RealVec> RealVec2;
typedef vector<RealVec2> RealVec3;

typedef vector<int> IntVec;
typedef vector<IntVec> IntVec2;
typedef vector<IntVec2> IntVec3;

template<class T> void AllocTwoVec(int dim1, int dim2, vector<vector<T> > * m) {
	m->clear();
	m->resize(dim1);
	for (int i = 0; i < dim1; ++i) {
		(*m)[i].resize(dim2);
	}
}

// Functions for converting from R structures to C/C++ data structures
int GetInt(SEXP p, int default_val, int* err_code);

double GetNumeric(SEXP p, double default_val, int* err_code);

SEXP ListEltByName(SEXP vec, const char * str, int * elt_idx);

void SaveIntVecToSxp(SEXP vec, int idx, const IntVec& v);
void SaveDblVecToSxp(SEXP vec, int idx, const RealVec& v);

bool SxpToIntVec(SEXP vec, IntVec* v);
bool SxpToDblVec(SEXP vec, RealVec* v);

void SxpVecEltToIntVec(SEXP vec, int idx, IntVec* v);
void SxpVecEltToIntVecs(SEXP vec, int idx, IntVec2* v);


void SxpVecEltToDblVec(SEXP vec, int idx, RealVec* v);

void SxpVecEltToDblVecs(SEXP vec, int idx, RealVec2* v);
int ListEltIdxByName(SEXP v, const char * name);

void GetMatrixDim(SEXP m, int * n_row, int * n_col);

void SetMatrixDim(SEXP m, int n_row, int n_col);


__inline void ZeroDblVec(RealVec * v) {
  for (int i = 0, sz = v->size(); i < sz; ++i) {
    (*v)[i] = 0.0;
  }
}

__inline double MaxDblVec(const RealVec& v) {
	double mx = R_NegInf;
	for (int i = 0, sz = v.size(); i < sz; ++i) {
		if (v[i] > mx) mx = v[i];
	}
	return(mx);
}

}

#endif