import numpy as np
import treelas as tl


def test_dual3(root=0):
    """Tree with 3 nodes"""
    parent = np.array([0, 0, 0], dtype=np.int32)
    x = np.array([-3., 1., 2])
    assert x.mean() == 0.0
    alpha = tl.tree_dual(parent, x)
    assert alpha.shape == x.shape
    assert np.isnan(alpha[root])
    assert root == 0
    assert (alpha[1:] == [-1., -2.]).all()
    assert x[root] == 0.0
