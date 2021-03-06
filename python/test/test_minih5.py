import pytest
import treelas as tl
import numpy as np
import sys
from data import data_dir


def load_mini_h5():
    h5py = pytest.importorskip("h5py")
    with h5py.File(data_dir('test', 'tree.mini.h5'), 'r') as io:
        y = io['y'][:]
        parent = io['parent'][:]
        xt = io['xt'][:]
        lam = io['lam'][()]
        root = int(io.attrs['root'])

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
    return y, parent, xt, lam, root


def test_mini_h5():
    y, parent, xt, lam, root = load_mini_h5()
    x = tl.tree_dp(y, parent, lam, root)
    assert (x.reshape(xt.shape) == xt).all()
    assert sys.getrefcount(parent) <= 2
    assert sys.getrefcount(y) <= 2
    assert sys.getrefcount(x) <= 2


def test_mini_h5_w():
    y, parent, xt, lam, root = load_mini_h5()
    n = len(y)
    mua = np.ones(n)
    lama = lam * np.ones(n)
    x2 = tl.tree_dp(y=y, parent=parent, lam=lama, mu=mua, root=root)
    assert (x2.reshape(xt.shape) == xt).all()


def test_gamma():
    y, parent, xt, lam, root = load_mini_h5()
    t = tl.tree.TreeInstance(y=y, parent=parent, lam=lam, root=root)
    x = t.solve().x
    assert np.abs(x - xt.flatten()).max() < 1e-14
    gam = t.gamma
    assert gam.min() >= -1e-14
    assert gam.max() <= +1e-14
