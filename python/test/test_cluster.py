import numpy as np
from treelas.idx import cluster
from test_biadjacent import square


def test_cluster_2x2():
    neigh = square()
    x = np.array([[+0.0, 0.1],
                  [-0.1, 1.1]], order='F')
    pidx = cluster(x, neigh, eps=0.2)
    # assert len(pidx) == 2
    parts = set(map(frozenset, (pidx[0], pidx[1])))
    expect = set(map(frozenset, ([3], [0, 1, 2])))
    assert parts == expect
