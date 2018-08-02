import numpy as np
from treelas.adjidx import ChildrenIndex


def test_children_root1():
    parent = np.array([6,  6,  6,  7,  7,  7,  8,  8,  8], dtype=np.int32)
    root = 8
    n = 9
    assert len(parent) == n
    assert parent[root] == root

    cix = ChildrenIndex(parent, root)
    for i in range(6):
        assert len(cix[i]) == 0

    assert list(cix[6]) == [0, 1, 2]
    assert list(cix[7]) == [3, 4, 5]
    assert list(cix[8]) == [6, 7]


def test_children_root2():
    parent = np.array(
        [6,  6,  6,  7,  7,  7,  8,  8, 17, 15, 15, 15, 16, 16, 16, 17, 17, 17],
        dtype=np.int32)
    root = 17
    cidx = ChildrenIndex(parent, root)
    assert len(cidx) == len(parent)
