from numba import njit


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
    idx = np.zeros(n+1, dtype=str(int64))
    i = _compute_segments(x, idx, eps)
    idx.resize(i+1)
    return idx
