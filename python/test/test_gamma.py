from treelas import TreeInstance
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
