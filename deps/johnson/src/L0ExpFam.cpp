#include <R.h>

#include "L0Seg.hpp"
#include "util.hpp"

SEXP
SaveMsg(L0Seg::L0ExpFamMsgElt *inp_segs, int n_inp_segs)
{
    Rprintf("saving message to sxp\n");

    if (n_inp_segs < 0) {
        Rprintf("SaveMsg : n_inp_segs = %d\n", n_inp_segs);
        return (R_NilValue);
    }
    SEXP x0, c0, lin0, quad0;
    PROTECT(x0 = NEW_NUMERIC(n_inp_segs));
    PROTECT(c0 = NEW_NUMERIC(n_inp_segs));
    PROTECT(lin0 = NEW_NUMERIC(n_inp_segs));
    PROTECT(quad0 = NEW_NUMERIC(n_inp_segs));

    for (int seg_idx = 0; seg_idx < n_inp_segs; ++seg_idx) {
        REAL(x0)[seg_idx] = inp_segs[seg_idx].x;
        REAL(c0)[seg_idx] = inp_segs[seg_idx].c;
        REAL(lin0)[seg_idx] = inp_segs[seg_idx].coef1;
        REAL(quad0)[seg_idx] = inp_segs[seg_idx].coef2;
    }

    SEXP ret_vec;
    PROTECT(ret_vec = allocVector(VECSXP, 4));

    SET_VECTOR_ELT(ret_vec, 0, x0);
    SET_VECTOR_ELT(ret_vec, 1, c0);
    SET_VECTOR_ELT(ret_vec, 2, lin0);
    SET_VECTOR_ELT(ret_vec, 3, quad0);
    UNPROTECT(5);
    return ret_vec;
}

extern "C" {

SEXP
EfamL0Vit(
    SEXP coefs_sxp,
    SEXP lambda2_sxp,
    SEXP retPath,
    SEXP maxSegs,
    SEXP avg_n_backptr_segs_sxp)
{
    using namespace L0Seg;

    int max_segs = Util::GetInt(maxSegs, 0, 0);
    double lam2 = Util::GetNumeric(lambda2_sxp, 0, 0);

    if (lam2 <= 0.0) {
        error("EfamL0Vit : 'lambda2_sxp' must be positive\n");
    }

    double param_bd[2] = {R_NegInf, R_PosInf};

    if (!IS_NUMERIC(coefs_sxp)) {
        error("EfamL0Vit : 'coefs_sxp' must be 2-by-n matrix of real numbers\n");
    } else if (LENGTH(coefs_sxp) % 2 != 0 || LENGTH(coefs_sxp) < 2) {
        error("EfamL0Vit : 'coefs_sxp' must be matrix with two rows\n");
    }

    double *x = REAL(coefs_sxp);
    int n_obs = LENGTH(coefs_sxp) / 2;

    int avg_n_backptr_segs = Util::GetInt(avg_n_backptr_segs_sxp, 0, 0);
    if (avg_n_backptr_segs <= 4) {
        error("EfamL0Vit : avg_n_backptr_segs_sxp <= 4.  This is likely too low\n");
    }

    int max_seg_U = max_segs + 30;
    int max_seg_U2 = max_segs + 20;

    L0ExpFamMsgElt *msg_buf = new L0ExpFamMsgElt[max_seg_U * 2];
    L0ExpFamMsgElt *vit_msg1 = msg_buf;
    L0ExpFamMsgElt *vit_msg2 = msg_buf + max_seg_U;
    double *back_ptrs_max = new double[n_obs * (1 + avg_n_backptr_segs)];
    double *back_ptr_segs = back_ptrs_max + n_obs;
    int *back_ptr_pos = new int[n_obs * 2];
    int *n_back_ptr_segs = back_ptr_pos + n_obs;

    EfamL0CleanupStruct bufs;
    bufs.msg_buf_ = msg_buf;
    bufs.dbl_buf_ = back_ptrs_max;
    bufs.int_buf_ = back_ptr_pos;

    back_ptr_pos[0] = n_back_ptr_segs[0] = 0;

    InitL0Msg(vit_msg1, param_bd[0], 0.0, x[0], x[1]);
    int vit1_len = 1, vit2_len = -1;

    if (x[0] == 0.0 && x[1] == 0.0) {
        L0VitCleanup(&bufs);
        error("invalid observation at index 0\n");
    }

    for (int i = 1; i < n_obs; i++) {
        double y_str = 0;
        GaussL0VitMsgMax(vit_msg1, vit1_len, back_ptrs_max + i, &y_str);

        if (y_str == R_NegInf) {
            Rprintf("Viterbi msg invalid (iter %d)\n", i);
            SEXP ret_sxp = SaveMsg(vit_msg1, vit1_len);
            L0VitCleanup(&bufs);
            return (ret_sxp);
        }

        double water_level = y_str - lam2;

        back_ptr_pos[i] = back_ptr_pos[i - 1] + 2 * n_back_ptr_segs[i - 1];

        int r1 = L0FloodSegments(
            vit_msg1,
            vit1_len,
            water_level,
            param_bd,
            vit_msg2,
            &vit2_len,
            max_seg_U2,
            back_ptr_segs + back_ptr_pos[i],
            n_back_ptr_segs + i);

        if (r1 != 1) {
            SEXP ret_sxp = SaveMsg(vit_msg1, vit1_len);
            L0VitCleanup(&bufs);
            return (ret_sxp);
        }

        if (back_ptr_pos[i] + 2 * n_back_ptr_segs[i] >= avg_n_backptr_segs * n_obs) {
            L0VitCleanup(&bufs);
            error("Not enough memory to store backpointer segments! (index %d)\n", i);

        } else if (vit2_len >= max_seg_U2) {
            L0VitCleanup(&bufs);
            error(
                "Not enough memory to store Viterbi message! (index %d # segs = %d / "
                "%d)\n",
                i,
                vit1_len,
                vit2_len);
        }

        L0AddTerms(vit_msg2, vit2_len, x[2 * i], x[2 * i + 1]);
        if (x[2 * i] == 0.0 && x[2 * i + 1] == 0.0) {
            L0VitCleanup(&bufs);
            error("invalid observation at index 0\n");
        }

        L0ExpFamMsgElt *vit_msg3 = vit_msg1;
        vit_msg1 = vit_msg2;
        vit_msg2 = vit_msg3;
        int vit3_len = vit1_len;
        vit1_len = vit2_len;
        vit2_len = vit3_len;
    }

    int backtrace_res = L0DualBacktrace(
        vit_msg1,
        vit1_len,
        back_ptr_segs,
        n_back_ptr_segs,
        back_ptr_pos,
        back_ptrs_max,
        n_obs,
        REAL(retPath));

    L0VitCleanup(&bufs);
    if (!backtrace_res) {
        error("Backtrace failed\n");
    }

    return R_NilValue;
}

void
_EfamL0VitByNseg(
    const int n_levels,
    const int avg_num_bpsegs,
    const int max_segs,
    const int n_obs,
    double *x,
    double *retPath)
{
    double param_bd[2] = {R_NegInf, R_PosInf};

    L0Seg::L0ByNSegData s;
    s.Alloc(n_levels, n_obs, avg_num_bpsegs, max_segs);

    for (int seg_idx = 0; seg_idx < s.n_levels_; ++seg_idx) {
        InitL0Msg(s.msgs1_[seg_idx], param_bd[0], 0.0, x[0], x[1]);
    }

    for (int i = 1; i < n_obs; ++i) {
        for (int seg_idx = n_levels - 1; seg_idx >= 0; --seg_idx) {
            double y_str = 0;
            double water_level = R_NegInf;

            if (seg_idx > 0 && seg_idx <= i) {
                GaussL0VitMsgMax(
                    s.msgs1_[seg_idx - 1],
                    s.msg1_lens_[seg_idx - 1],
                    &s.bp_max_[seg_idx][i],
                    &y_str);

                if (y_str == R_NegInf) {
                    s.Cleanup();
                    error("Viterbi msg invalid (iter %d)\n", i);
                }
                water_level = y_str;
            }

            s.bp_posns_[seg_idx][i] =
                s.bp_posns_[seg_idx][i - 1] + 2 * s.bp_szs_[seg_idx][i - 1];

            if (s.bp_posns_[seg_idx][i] < 0)
                error("xxx");

            int r1 = 1;
            if (seg_idx == i) {
                s.InitMsg(seg_idx, water_level, param_bd);
            } else {
                r1 = L0FloodSegments(
                    s.msgs1_[seg_idx],
                    s.msg1_lens_[seg_idx],
                    water_level,
                    param_bd,
                    s.msgs2_[seg_idx],
                    &s.msg2_lens_[seg_idx],
                    s.msg_size_,
                    &s.bp_segs_[seg_idx][s.bp_posns_[seg_idx][i]],
                    &s.bp_szs_[seg_idx][i]);
            }

            if (r1 < 0) {
                s.Cleanup();
                if (r1 == -1)
                    error("Numerical error.  (index %d)\n", i);
                if (r1 < -1)
                    error("Allocate more memory. (index %d)\n", i);
            }

            s.MemoryCheck(seg_idx, i);
            L0AddTerms(
                s.msgs2_[seg_idx], s.msg2_lens_[seg_idx], x[2 * i], x[2 * i + 1]);
            s.SwapMsg(seg_idx);
        }
    }

    s.BackTrace(retPath);
    s.Cleanup();
}

SEXP
EfamL0VitByNseg(
    SEXP coef_sxp,
    SEXP num_segments_sxp,
    SEXP retPath_sxp,
    SEXP max_msg_sz_sxp,
    SEXP avg_num_bpsegs_sxp)
{
    using namespace L0Seg;

    const int n_levels = Util::GetInt(num_segments_sxp, 0, 0);
    const int avg_num_bpsegs = Util::GetInt(avg_num_bpsegs_sxp, 0, NULL);
    const int max_segs = Util::GetInt(max_msg_sz_sxp, 0, NULL);
    const int n_obs = LENGTH(coef_sxp) / 2;
    double *x = REAL(coef_sxp);
    double *retPath = REAL(retPath_sxp);

    _EfamL0VitByNseg(n_levels, avg_num_bpsegs, max_segs, n_obs, x, retPath);

    return R_NilValue;
}

}
