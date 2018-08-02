import numpy as np
from treelas.children import PyChildrenIndex as ChildrenIndex


def test_children_root1():
    parent = np.array([6,  6,  6,  7,  7,  7,  8,  8,  8])
    root = 8
    n = 9
    assert len(parent) == n
    assert parent[root] == root

    idx_x = np.array([0] * 7 + [3, 6, 8])
    pi_x  = np.array(range(n))

    cix = ChildrenIndex(idx=idx_x, pi=pi_x)
    for i in range(6):
        assert len(cix[i]) == 0

    assert (cix[6] == [0, 1, 2]).all()
    assert (cix[7] == [3, 4, 5]).all()
    assert (cix[8] == [6, 7]).all()

    idx = np.zeros(n+1, dtype=int)
    for p in parent:
        idx[p] += 1
    idx[root] -= 1
    # now, idx is the number of children (where root is count as self child)
    assert (idx == np.array([0, 0, 0, 0, 0, 0, 3, 3, 2, 0])).all()
    # where we aim at
    # assert (np.cumsum(idx)[:-1] == idx_x[1:]).all()
    acc = 0
    deg_i = 0
    deg_ii = idx[0]
    for i in range(n):
        idx[i] = acc
        acc += deg_i
        deg_i = deg_ii
        deg_ii = idx[i+1]
    idx[n] = acc

    pi = -np.ones(n, dtype=int)
    pi[-1] = root
    for v in range(n):
        p = parent[v]
        if v == p:
            continue
        pi[idx[p+1]] = v
        idx[p+1] += 1

    assert idx[n] == n-1, f'idx[n] = {idx[n]} != {n-1} = n-1'

    assert (idx == idx_x).all()
    assert (pi == pi_x).all()


def test_children_root2():
    parent = np.array(
        [6,  6,  6,  7,  7,  7,  8,  8, 17, 15, 15, 15, 16, 16, 16, 17, 17, 17])
    root = 17
    cidx = ChildrenIndex.compute(parent, root)
