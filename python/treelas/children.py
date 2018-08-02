import numpy as np
try:
    from numba import njit
except ImportError:
    import warnings

    warnings.warn("Please install `numba` for fast computation")

    def njit(f):
        """Fake just-in-time compilation"""
        return f


class PyChildrenIndex:
    """Similar to the C++ code"""
    def __init__(self, idx, pi):
        self.idx = idx
        self.pi = pi

    def __getitem__(self, i):
        return self.pi[self.idx[i]:self.idx[i+1]]

    @staticmethod
    def compute(parent, root=0):
        @njit(cache=True)
        def _compute(n, parent, root, idx, pi):
            for p in parent:
                idx[p] += 1
            idx[root] -= 1
            acc = 0
            deg_i = 0
            deg_ii = idx[0]
            for i in range(n):
                idx[i] = acc
                acc += deg_i
                deg_i = deg_ii
                deg_ii = idx[i+1]
                idx[n] = acc
            pi[-1] = root
            for v in range(n):
                p = parent[v]
                if v == p:
                    continue
                pi[idx[p+1]] = v
                idx[p+1] += 1

        assert parent[root] == root
        n = len(parent)

        idx = np.zeros(n+1, dtype=int)
        pi = -np.ones(n, dtype=int)
        _compute(n, parent, root, idx, pi)
        assert idx[n] == n-1, f'idx[n] = {idx[n]} != {n-1} = n-1'
        assert pi[-1] == root
        return PyChildrenIndex(idx=idx, pi=pi)
