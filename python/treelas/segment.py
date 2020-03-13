import numpy as np
from graphidx.py._numba import njit
from ._treelas import line_dp as line_las


def line_segments(x, eps=1e-10):
    """Return indices idx where x changes, i.e. for all i=0..len(x)-1:
        x[idx[i]:idx[i+1]] is constant
    """
    @njit(cache=True)
    def _compute_segments(x, idx, eps):
        i = 0
        idx[i] = 0
        n = len(x)
        for j in range(n):
            if abs(x[idx[i]] - x[j]) > eps:
                i += 1
                idx[i] = j
        i += 1
        idx[i] = n
        return i

    n = len(x)
    idx = np.zeros(n+1, dtype=int)
    i = _compute_segments(x, idx, eps)
    idx.resize(i+1)
    return idx


def mean_segments(y, segs, x=None):
    if x is None:
        x = np.zeros_like(y)
    for i in range(len(segs)-1):
        a, b = segs[i], segs[i+1]
        x[a:b] = y[a:b].mean()
    return x


def line_mean(y, lam, eps=1e-10, x=None):
    if x is None:
        x = np.zeros_like(y)
    line_las(y, lam=lam, out=x)
    segs = line_segments(x, eps=eps)
    return mean_segments(y, segs, x)
