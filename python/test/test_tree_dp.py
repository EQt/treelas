import numpy as np
from treelas import Tree, TreeInstance


def test_demo_7x3():
    y = np.fromstring("0.62 0.73 0.71 1.5 1.17 0.43 1.08 0.62 " +
                      "1.73 0.95 1.46 1.6 1.16 0.38 0.9 0.32 " +
                      "-0.48 0.95 1.08 0.02 0.4", sep=" ")
    parent = np.array([0, 4, 5, 0, 3, 4, 7, 8, 5, 6, 7, 8,
                       9, 14, 17, 12, 15, 16, 19, 16, 17])
    prob = TreeInstance(y, parent, lam=1.0)
    assert prob.root == 0
    assert prob.parent.dtype == np.int32
    assert abs(prob.x.mean() - prob.y.mean()) < 1e-15
    assert len(np.unique(prob.x)) == 2
