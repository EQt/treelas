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



class L0ByNSegData
{
  public:
    int n_levels_, n_obs_, avg_n_bpsegs_, msg_size_;

    L0ExpFamMsgElt *msg_buf_;
    std::vector<L0ExpFamMsgElt *> msgs1_, msgs2_;
    Util::IntVec msg1_lens_, msg2_lens_;

    Util::RealVec2 bp_max_, bp_segs_;
    Util::IntVec2 bp_posns_, bp_szs_;

  public:
    L0ByNSegData() { this->Init(); }

    void Cleanup()
    {
        delete msg_buf_;
        msg_buf_ = NULL;
        this->Init();
    }

    void Init()
    {
        msg_buf_ = NULL;
        msg1_lens_.clear();
        msg2_lens_.clear();
        msgs1_.clear();
        msgs2_.clear();

        bp_max_.clear();
        bp_segs_.clear();
        bp_posns_.clear();
        bp_szs_.clear();

        n_levels_ = n_obs_ = msg_size_ = avg_n_bpsegs_ = 0;
    }

    void SwapMsg(int seg_idx)
    {
        L0ExpFamMsgElt *tmp_msg = msgs1_[seg_idx];
        msgs1_[seg_idx] = msgs2_[seg_idx];
        msgs2_[seg_idx] = tmp_msg;

        if (msg1_lens_[seg_idx] < 0 || msg2_lens_[seg_idx] < 0) {
            error("invalid message lengths (segment index = %d\n", seg_idx);
        }
        int tmp_len = msg1_lens_[seg_idx];
        msg1_lens_[seg_idx] = msg2_lens_[seg_idx];
        msg2_lens_[seg_idx] = tmp_len;
    }

    void Alloc(int n_levels, int n_obs, int avg_n_bpsegs, int msg_size)
    {
        int sz2 = msg_size * 2;
        if (sz2 < msg_size + 20)
            sz2 = msg_size + 20;

        msg_size_ = msg_size;
        n_obs_ = n_obs;
        n_levels_ = n_levels;
        avg_n_bpsegs_ = avg_n_bpsegs;

        msg_buf_ = NULL;

        int tot_knots = sz2 * 2 * n_levels_;
        msg_buf_ = new L0ExpFamMsgElt[tot_knots];
        L0ExpFamMsgElt *msg_buf_cur = msg_buf_;

        msgs1_.clear();
        msgs2_.clear();

        for (int seg_idx = 0; seg_idx < n_levels_; ++seg_idx) {
            msgs1_.push_back(msg_buf_cur);
            msg_buf_cur += sz2;
            msgs2_.push_back(msg_buf_cur);
            msg_buf_cur += sz2;
        }

        Util::AllocTwoVec(n_levels_, n_obs_, &bp_max_);
        Util::AllocTwoVec(n_levels_, avg_n_bpsegs_ * n_obs_, &bp_segs_);
        Util::AllocTwoVec(n_levels_, n_obs_, &bp_posns_);
        Util::AllocTwoVec(n_levels_, n_obs_, &bp_szs_);

        msg1_lens_.clear();
        msg2_lens_.clear();
        msg1_lens_.reserve(n_levels_);
        msg2_lens_.reserve(n_levels_);

        for (int seg_idx = 0; seg_idx < n_levels_; ++seg_idx) {
            bp_posns_[seg_idx][0] = bp_posns_[seg_idx][0] = 0;

            msg1_lens_.push_back(1);
            msg2_lens_.push_back(-1);
        }
    }

    void InitMsg(int seg_idx, double val, double *param_bd)
    {
        InitL0Msg(msgs1_[seg_idx], param_bd[0], val, 0.0, 0.0);
        InitL0Msg(msgs2_[seg_idx], param_bd[0], val, 0.0, 0.0);
        msg1_lens_[seg_idx] = msg2_lens_[seg_idx] = 1;

        bp_szs_[seg_idx][seg_idx] = 1;
        bp_segs_[seg_idx][bp_posns_[seg_idx][seg_idx]] = param_bd[0];
        bp_segs_[seg_idx][bp_posns_[seg_idx][seg_idx] + 1] = param_bd[1];
    }

    void MemoryCheck(int seg_idx, int obs_idx)
    {
        int next_posn = bp_posns_[seg_idx][obs_idx] + 2 * bp_szs_[seg_idx][obs_idx];
        int max_posn = avg_n_bpsegs_ * n_obs_;

        if (next_posn >= max_posn || msg2_lens_[seg_idx] >= msg_size_) {
            this->Cleanup();
            error("Allocate more memory.\n");
        }
    }

    void BackTrace(double *path)
    {
        int jump_idx = n_levels_ - 1;
        GaussL0VitMsgMax(
            msgs1_[jump_idx], msg1_lens_[jump_idx], path + (n_obs_ - 1), NULL);

        double last_val = path[n_obs_ - 1];

        for (int i = n_obs_ - 2; i >= 0; --i) {
            double *bp = &bp_segs_[jump_idx][bp_posns_[jump_idx][i + 1]];
            int n_sg = bp_szs_[jump_idx][i + 1];

            int in_seg = 0;
            for (int k = 0; k < n_sg; k++) {
                if (bp[2 * k] > last_val) {
                    break;
                } else if (bp[2 * k] <= last_val && bp[2 * k + 1] >= last_val) {
                    in_seg = 1;
                    break;
                }
            }
            if (in_seg) {
                if (jump_idx) {
                    last_val = path[i] = bp_max_[jump_idx][i + 1];
                    --jump_idx;
                } else {
                    this->Cleanup();
                    error("invalid jump at index %d\n", i);
                }
            } else {
                last_val = path[i] = path[i + 1];
            }
        }
    }


    ~L0ByNSegData() { this->Cleanup(); }
};


void
EfamL0VitByNseg(
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

} // namespace L0Seg
