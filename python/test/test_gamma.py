from treelas import TreeInstance, post_order
import numpy as np


def test_gamma3():
    """Call TreeInstance.gamma on a simple tree"""
    t = TreeInstance(y=np.array( [0., 13, 0.75]),
                     mu=np.array([1., 0.1, 0.001]),
                     lam=np.array([1.0, 0.5, 0.3]),
                     parent=np.array([1, 2, 2], dtype=np.int32),
                     root=2)
    g = t.gamma
    assert (g >= 0).all(), f'gamma={g}'
    assert (g < 1e-10).all(), f'gamma={g}'


def test_gamma_line3(eps=1e-14):
    ti = TreeInstance(root=2,
                      y = np.array([ 0.8, -0.6,  0. ]),
                      mu = 1.0,
                      lam = 0.2,
                      parent = np.array([2, 0, 2], dtype=np.int32))

    po = post_order(ti.parent, include_root=True)
    assert (po == [1, 0, 2]).all()

    ti.solve()
    assert ((ti.x - ti.y)[po] / ti.lam).astype(int).tolist() == [1, -2, 1]

    assert ti.gamma.min() >= -eps, ti.gamma.min()
    assert ti.gamma.max() <= +eps, f'gamma = {ti.gamma.max()}\n{ti}'
