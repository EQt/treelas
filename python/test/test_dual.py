import numpy as np
import treelas as tl
from pytest import approx


def test_dual3(root=0):
    """Tree with 3 nodes"""
    parent = np.array([0, 0, 0], dtype=np.int32)
    x = np.array([-3., 1., 2])
    assert x.mean() == 0.0
    z = x.copy()
    alpha = tl.tree_dual(parent, z)
    assert alpha.shape == x.shape
    assert np.isnan(alpha[root])
    assert root == 0
    assert (alpha[1:] == [+1., +2.]).all()
    assert z[root] == 0.0


def test_root_sum(n=10, seed=42):
    parent = np.arange(-1, n-1, dtype=np.int32)
    parent[0] = 0
    root = tl.find_root(parent)
    assert root == 0
    np.random.seed(seed)
    x = np.random.uniform(size=n)
    z = x.copy()
    alpha = tl.tree_dual(parent, z)
    assert x.sum() == approx(z[root])
    x_rev = np.flipud(x)
    c = np.cumsum(x_rev)
    assert np.flipud(c)  == approx(z)
    assert x.sum() + alpha[1:].sum() == approx(z.sum(), rel=1e-4), \
        f'\nrhs={x.sum() - alpha[1:].sum()}\nlhs={z.sum()}'
