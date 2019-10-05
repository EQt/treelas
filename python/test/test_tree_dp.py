import numpy as np
from treelas import post_order, TreeInstance


def test_demo_3x7_postord():
    parent = np.array([0, 4, 5, 0, 3, 4, 7, 8, 5, 6, 7, 8,
                       9, 14, 17, 12, 15, 16, 19, 16, 17])
    po = post_order(parent, include_root=True)
    expect = np.array([12, 11, 19, 20, 21, 14, 15, 18, 17, 16, 13,
                       10, 7, 8, 9, 3, 6, 2, 5, 4, 1], dtype='i4') - 1
    assert (po == expect).all()


def test_demo_3x7():
    y = np.fromstring("0.62 0.73 0.71 1.5 1.17 0.43 1.08 0.62 " +
                      "1.73 0.95 1.46 1.6 1.16 0.38 0.9 0.32 " +
                      "-0.48 0.95 1.08 0.02 0.4", sep=" ")
    parent = np.array([0, 4, 5, 0, 3, 4, 7, 8, 5, 6, 7, 8,
                       9, 14, 17, 12, 15, 16, 19, 16, 17])
    lam = 1.0
    prob = TreeInstance(y, parent, lam=lam)
    assert prob.root == 0
    assert prob.parent.dtype == np.int32
    prob.solve()
    assert abs(prob.x.mean() - prob.y.mean()) < 1e-15
    assert len(np.unique(prob.x)) == 2
    assert max(np.abs(prob.dual[2:]) - lam) < 1e-12
    assert max(np.abs(prob.gamma)) < 1e-15
