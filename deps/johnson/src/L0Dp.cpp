#include <R.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <R_ext/Print.h>

#include <vector>

#include "util.hpp"

// This is an implementation of Bellman's algorithm for segmentation.

using namespace std;

namespace L0Dp {

using namespace Util;

const int POIS_LIK = 0;
const int GAUSS_LIK = 1;

class Segmenter {
public:
  Segmenter(const RealVec& o) : obs_(o) {
    this->CalcCumsums();
  }

  void Segment(int n_seg, IntVec * seg_bds) {
    DpMat dp_mat;
    this->AllocDpMat(n_seg, &dp_mat);

    int nobs = obs_.size();
    for (int j = 0; j < nobs; ++j) {
      dp_mat[0][j].second = this->GetSegmentLik(0, j);
      dp_mat[0][j].first = -1;
    }

    for (int i = 1; i < n_seg; ++i) {
      for (int j = 0; j < nobs; ++j) {
        int best_idx = -1;
        double best_val = R_NegInf;

        for (int k = 0; k < j; ++k) {
          double new_val = this->GetSegmentLik(k+1, j) + dp_mat[i-1][k].second;
          if (new_val > best_val) {
            best_val = new_val;
            best_idx = k;
          }
        }

        dp_mat[i][j].second = best_val;
        dp_mat[i][j].first = best_idx;
      }
    }

    *seg_bds = IntVec(n_seg-1);
    int cur_pt = nobs - 1;
    for (int i = n_seg - 1; i > 0; --i) {
      int next_pt = dp_mat[i][cur_pt].first;
      (*seg_bds)[i-1] = next_pt;
      cur_pt = next_pt;
    }
  }

  virtual ~Segmenter() { }
protected:
  double get_cumsum(int i) const {
    return cumsums_[i];
  }

  typedef pair<int, double> DpElt;
  typedef vector<vector<DpElt> > DpMat;

  virtual double GetSegmentLik(int lb, int ub) const = 0;

  void CalcCumsums() {
    double s = 0.0;
    cumsums_ = RealVec(obs_.size());
    for (int i = 0, n = obs_.size(); i < n; ++i) {
      s += obs_[i];
      cumsums_[i] = s;
    }
  }

  void AllocDpMat(int n_seg, DpMat * dp_mat) const {
    vector<DpElt> v(obs_.size(), DpElt(-1, R_NegInf));
    *dp_mat = DpMat(n_seg, v);
  }

  RealVec cumsums_;
  RealVec obs_;
};

class PoisSegmenter : public Segmenter {
public:
  PoisSegmenter(const RealVec& o) : Segmenter(o) { }
private:
  virtual double GetSegmentLik(int lb, int ub) const {
    double s0 = (lb) ? this->get_cumsum(lb-1) : 0.0;
    double s1 = this->get_cumsum(ub);

    double npt = (double)(ub - lb + 1);
    double obs_sum = (s1 - s0);
    double beta_hat = obs_sum / npt;

    if (beta_hat == 0.0) {
      return(0.0);
    } else {
      return(-npt * beta_hat + obs_sum * log(beta_hat));
    }
  }
};

class GaussSegmenter : public Segmenter {
public:
  GaussSegmenter(const RealVec& o) : Segmenter(o) { }
private:
  virtual double GetSegmentLik(int lb, int ub) const {
    double s0 = (lb) ? this->get_cumsum(lb-1) : 0.0;
    double s1 = this->get_cumsum(ub);

    double npt = (double)(ub - lb + 1);
    double obs_sum = (s1 - s0);
    double beta_hat = obs_sum / npt;

    if (beta_hat == 0.0) {
      return(0.0);
    } else {
      return(beta_hat * (2.0 * obs_sum - beta_hat));
    }
  }
};

}  // namespace

extern "C" {


SEXP L0DpSegment(SEXP o, SEXP lik_type, SEXP n_seg) {
  using namespace Util;
  using namespace L0Dp;

  RealVec v;
  SxpToDblVec(o, &v);

  L0Dp::Segmenter * alg = NULL;

  int lik_code = GetInt(lik_type, GAUSS_LIK, NULL);
  int ns = GetInt(n_seg, 2, NULL);

  switch (lik_code) {
  case POIS_LIK:
    alg = new PoisSegmenter(v);
    break;
  case GAUSS_LIK:
    alg = new GaussSegmenter(v);
    break;
  default:
    error("L0Dp : unknown likelihood code (%d)\n", INTEGER(lik_type)[0]);
  }

  if (alg) {
    IntVec seg_bds;
    alg->Segment(ns, &seg_bds);

    SEXP ret_sxp = NEW_INTEGER(ns-1);
    for (int i = 0; i + 1 < ns; ++i) {
      INTEGER(ret_sxp)[i] = seg_bds[i];
    }
    delete alg;
    return ret_sxp;
  } else {
    return R_NilValue;
  }
}

}
