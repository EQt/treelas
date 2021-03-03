#pragma once

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

} // namespace L0Seg
