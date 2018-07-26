import treelas as tl
import h5py
from os import path
import numpy as np
import sys
import gc


def load_mini_h5():
    mini_h5 = path.join(path.dirname(__file__), '..', '..', 'data', 'tree.mini.h5')
    assert path.exists(mini_h5)
    with h5py.File(mini_h5) as io:
        y = io['y'].value
        parent = io['parent'].value
        xt = io['xt'].value
        lam = io['lam'].value
        root = int(io.attrs['root'])
        mu = 1.0

    y = y.reshape(-1)
    n = len(y)
    parent = parent.astype(np.int32)
    assert len(parent) == n
    if parent.min() > 0:
        parent -= parent.min()
        root -= 1

    assert parent[root] == root
    assert sys.getrefcount(parent) <= 2
    assert sys.getrefcount(y) <= 2
    return y, parent, xt, lam, root, mu


def test_mini_h5():
    y, parent, xt, lam, root, mu = load_mini_h5()
    x = tl.tree_dp(y, parent, lam, mu, root)
    assert (x.reshape(xt.shape) == xt).all()
    assert sys.getrefcount(parent) <= 2
    assert sys.getrefcount(y) <= 2
    assert sys.getrefcount(x) <= 2


def test_mini_h5_w():
    y, parent, xt, lam, root, mu = load_mini_h5()
    x1 = tl.tree_dp(y, parent, lam, mu, root)
    n = len(y)
    mua = 1.0 * mu * np.ones(n)
    lama = lam * np.ones(n)
    x2 = tl.tree_dp_w(y=y, parent=parent, lam=lama, mu=mua, root=root)
    assert (x2.reshape(xt.shape) == xt).all()
