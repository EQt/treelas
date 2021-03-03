// This is an implementation of the DpSegPen and DpSegN segmentation algorithms
// given in the paper.
#include <vector>
#include <error.h>

#include "util.hpp"
#include "L0Seg.hpp"

namespace L0Seg {

double
ClampVal(double x, double lwr, double upr)
{
    if (x < lwr)
        return lwr;
    if (x > upr)
        return upr;
    return x;
}

int
GaussLikSolve(double a, double b, double *pts)
{
    // solves ax + x^2 = b
    // The quadratic formula gives
    //   (-a \pm sqrt(a*a + 4b)) / 2
    pts[0] = pts[1] = R_NegInf;

    double d = a * a + 4.0 * b;

    if (d < 0.0) {
        return 0;
    }

    d = sqrt(d);
    double r1 = 0.5 * (-a + d);
    double r2 = 0.5 * (-a - d);

    if (r1 < r2) {
        pts[0] = r1;
        pts[1] = r2;
    } else {
        pts[0] = r2;
        pts[1] = r1;
    }
    return 1;
}

// --------------------------------------------------------------

void
L0AddTerms(L0ExpFamMsgElt *inp_segs, int n_inp_segs, double coef1, double coef2)
{
    for (int i = 0; i < n_inp_segs; i++, ++inp_segs) {
        inp_segs->coef1 += coef1;
        inp_segs->coef2 += coef2;
    }
}

void
EfamDebugPrintMsg(L0ExpFamMsgElt *inp_segs, int n_inp_segs)
{
    for (int j = 0; j < n_inp_segs; ++j, ++inp_segs) {
        Rprintf(
            "%d) x = %g, const = %g, coef1 = %g, coef2 = %g\n",
            j,
            inp_segs->x,
            inp_segs->c,
            inp_segs->coef1,
            inp_segs->coef2);
    }
}

void
GaussL0VitMsgEval(L0ExpFamMsgElt *inp_segs, int n_inp_segs, double x0, double *y_opt)
{
    L0ExpFamMsgElt *is = inp_segs + (n_inp_segs - 1);
    double next_x = R_PosInf;

    for (int j = 0; j < n_inp_segs; ++j, --is) {
        if (is->x <= x0 && x0 <= next_x) {
            if (y_opt) {
                *y_opt = is->c + x0 * (is->coef1 + is->coef2 * x0);
            }
            return;
        }
        next_x = is->x;
    }
}

void
GaussL0VitMsgMax(L0ExpFamMsgElt *inp_segs, int n_inp_segs, double *x_opt, double *y_opt)
{
    L0ExpFamMsgElt *is = inp_segs + (n_inp_segs - 1);
    double x_best = R_NegInf, y_best = R_NegInf;

    double x_max = 0, y_max = 0, next_x = R_PosInf;

    for (int j = 0; j < n_inp_segs; ++j, --is) {
        // we need to maximize f(z) = a + b * z + c * z^2
        // 0 = f'(z) = b + 2cz
        //   yields z0 = -b/(2c)
        // We should evaluate f(z0) as
        //   a + z0 * (b + c * z0)
        // This is a guassian likelihood (possibly weighted)
        // so we know c is always negative and never zero

        double x = is->x;
        double a = is->c;
        double b = is->coef1;
        double c = is->coef2;

        x_max = -b / (2.0 * c);
        if (x_max < x) {
            x_max = x;
        } else if (x_max > next_x) {
            x_max = next_x;
        }
        y_max = a + x_max * (b + c * x_max);

        if (y_max > y_best) {
            y_best = y_max;
            x_best = x_max;
        }
        next_x = x;
    }
    if (x_opt)
        *x_opt = x_best;
    if (y_opt)
        *y_opt = y_best;
}

// --------------------------------------------------------------

double
GaussL0KnotEval(L0ExpFamMsgElt *seg, double x)
{
    return (seg->c + x * (seg->coef1 + x * seg->coef2));
}

// --------------------------------------------------------------

void
SetConstSeg(L0ExpFamMsgElt **seg, int *seg_idx, double x, double val)
{
    L0ExpFamMsgElt *s = *seg;
    s->x = x;
    s->c = val;
    s->coef1 = s->coef2 = 0.0;
    *seg += 1;
    *seg_idx += 1;
}

void
SetMsgCoefs(L0ExpFamMsgElt **seg, int *seg_idx, double x, L0ExpFamMsgElt *coefs)
{
    L0ExpFamMsgElt *s = *seg;
    s->x = x;
    s->c = coefs->c;
    s->coef1 = coefs->coef1;
    s->coef2 = coefs->coef2;
    *seg += 1;
    *seg_idx += 1;
}

void
SetMsg(
    L0ExpFamMsgElt **seg,
    int *seg_idx,
    double x,
    double const_term,
    double coef1,
    double coef2)
{
    L0ExpFamMsgElt *s = *seg;
    s->x = x;
    s->c = const_term;
    s->coef1 = coef1;
    s->coef2 = coef2;
    *seg += 1;
    *seg_idx += 1;
}

void
InitL0Msg(L0ExpFamMsgElt *seg, double x, double const_term, double coef1, double coef2)
{
    seg->x = x;
    seg->c = const_term;
    seg->coef1 = coef1;
    seg->coef2 = coef2;
}

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
    int *n_backptr_ints)
{
    if (max_newseg_len < 0)
        error("max_newseg_len < 0\n");

    if (water_level == R_NegInf) {
        memcpy(new_seg, cur_seg, sizeof(L0ExpFamMsgElt) * cur_seg_len);
        if (n_backptr_ints)
            *n_backptr_ints = 0;
        if (newseg_len)
            *newseg_len = cur_seg_len;
        return 1;
    }

    int below_water = 0;

    double *bp = backptr_ints;
    int bp_idx = 0;

    double pts[2] = {0.0, 0.0};
    L0ExpFamMsgElt *is = cur_seg;
    L0ExpFamMsgElt *os = new_seg;
    int os_len = 0;

    // We check if we are starting below the threshold
    // and if we are we start with a constant segment

    double x = is->x;
    SetConstSeg(&os, &os_len, x, water_level);
    if (bp)
        bp[bp_idx++] = x;
    below_water = 1;

    for (int seg_idx = 0; seg_idx < cur_seg_len; ++seg_idx, ++is) {
        if (os_len >= max_newseg_len) {
            error("Out of memory\n");
        }
        x = is->x;
        double x_next = (seg_idx + 1 < cur_seg_len) ? is[1].x : param_bd[1];

        double dnm = (is->coef2 == 0.0) ? 1.0 : is->coef2;
        GaussLikSolve(is->coef1 / dnm, (water_level - is->c) / dnm, pts);

        if (below_water && pts[0] != R_NegInf && pts[0] < x_next && pts[0] > x - 1e-9) {
            // we exit
            SetMsgCoefs(&os, &os_len, pts[0], is);
            if (bp)
                bp[bp_idx++] = pts[0];
            below_water = 0;
        } else if (below_water == 0) {
            SetMsgCoefs(&os, &os_len, x, is);
        }

        if (below_water == 0 && pts[1] != R_NegInf && pts[1] < x_next && pts[1] > x) {
            SetConstSeg(&os, &os_len, pts[1], water_level);
            if (bp)
                bp[bp_idx++] = pts[1];
            below_water = 1;
        }
    }

    if (bp && 1 == (bp_idx % 2)) {
        // The last back-pointer interval is still open
        // We tie it up here
        bp[bp_idx++] = param_bd[1];
    }
    if (n_backptr_ints)
        *n_backptr_ints = bp_idx / 2;
    if (newseg_len)
        *newseg_len = os_len;

    return 1;
}

int
L0DualBacktrace(
    L0ExpFamMsgElt *final_msg,
    int final_msg_len,
    double *backptr_segs,
    int *num_backptr_segs,
    int *backptr_offsets,
    double *msg_maxs,
    int seq_len,
    double *fit)
{
    GaussL0VitMsgMax(final_msg, final_msg_len, fit + (seq_len - 1), NULL);

    if (!R_FINITE(fit[seq_len - 1])) {
        Rprintf("last elt invalid\n");
        return 0;
    }

    for (int i = seq_len - 2; i >= 0; i--) {
        double *bp = backptr_segs + backptr_offsets[i + 1];
        int n_sg = num_backptr_segs[i + 1];

        int in_seg = 0;
        for (int k = 0; k < n_sg; k++) {
            if (bp[2 * k] > fit[i + 1]) {
                break;
            } else if (bp[2 * k] <= fit[i + 1] && bp[2 * k + 1] >= fit[i + 1]) {
                in_seg = 1;
                break;
            }
        }
        if (in_seg) {
            fit[i] = msg_maxs[i + 1];
        } else {
            fit[i] = fit[i + 1];
        }
    }
    return 1;
}

void
L0VitCleanup(EfamL0CleanupStruct *s)
{
    delete s->msg_buf_;
    delete s->dbl_buf_;
    delete s->int_buf_;
}


} // namespace L0Seg
