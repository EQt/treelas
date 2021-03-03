#pragma once
#include <vector>

#include "util.hpp"

namespace L0Seg {

#define DEFAULT_TOL1 1e-6
#define DEFAULT_TOL2 1e-5
#define CONVERGENCE_FAILURE 2
#define WATER_LEV_FUDGE 1e-7


struct L0ExpFamMsgElt
{
    double x;
    double c;
    double coef1, coef2;
};


struct EfamL0CleanupStruct
{
    L0ExpFamMsgElt *msg_buf_;
    double *dbl_buf_;
    int *int_buf_;
};


void
InitL0Msg(L0ExpFamMsgElt *seg, double x, double const_term, double coef1, double coef2);


void
L0VitCleanup(EfamL0CleanupStruct *s);


void
GaussL0VitMsgMax(
    L0ExpFamMsgElt *inp_segs, int n_inp_segs, double *x_opt, double *y_opt);

int
L0FloodSegments(
    L0ExpFamMsgElt *cur_seg,
    int cur_seg_len,
    double water_level,
    double *param_bd,
    L0ExpFamMsgElt *new_seg,
    int *newseg_len,
    int max_newseg_len,
    double *backptr_ints,
    int *n_backptr_ints);


void
L0AddTerms(L0ExpFamMsgElt *inp_segs, int n_inp_segs, double coef1, double coef2);

int
L0DualBacktrace(
    L0ExpFamMsgElt *final_msg,
    int final_msg_len,
    double *backptr_segs,
    int *num_backptr_segs,
    int *backptr_offsets,
    double *msg_maxs,
    int seq_len,
    double *fit);


void
EfamL0VitByNseg(
    const int n_levels,
    const int avg_num_bpsegs,
    const int max_segs,
    const int n_obs,
    double *x,
    double *retPath);


} // namespace L0Seg
