import numpy as np
from treelas.idx import cluster
from test_biadjacent import square


def test_cluster_2x2_3vs1():
    neigh = square()
    x = np.array([+0.0, 0.1, -0.1, 1.1])
    pidx = cluster(x, neigh, eps=0.2)
    assert len(pidx) == 2
    parts = set(map(frozenset, (pidx[0], pidx[1])))
    expect = set(map(frozenset, ([3], [0, 1, 2])))
    assert parts == expect


def test_cluster_2x2_2vs2_h():
    neigh = square()
    x = np.array([+0.0, 0.1, 0.95, 1.1])
    pidx = cluster(x, neigh, eps=0.2)
    assert len(pidx) == 2
    parts = set(map(frozenset, (pidx[0], pidx[1])))
    expect = set(map(frozenset, ([0, 1], [2, 3])))
    assert parts == expect


def test_cluster_2x2_2vs2_v():
    neigh = square()
    x = np.array([+0.0, 0.99, 0.15, 1.1])
    pidx = cluster(x, neigh, eps=0.2)
    assert len(pidx) == 2
    parts = set(map(frozenset, (pidx[0], pidx[1])))
    expect = set(map(frozenset, ([0, 2], [1, 3])))
    assert parts == expect, f"{x[list(pidx[0])]} vs {x[list(pidx[1])]}, {neigh}"


def test_cluster_2x2_1vs1vs1vs2():
    neigh = square()
    x = np.array([-0.9, 0.01, 1.15, 0.1])
    pidx = cluster(x, neigh, eps=0.2)
    assert len(pidx) == 3
    parts = set(map(frozenset, (pidx[0], pidx[1], pidx[2])))
    expect = set(map(frozenset, ([0], [1, 3], [2])))
    assert parts == expect


if __name__ == '__main__':
    test_cluster_2x2()
