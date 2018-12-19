import numpy as np
from treelas.idx import ChildrenIndex


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
    assert list(cidx[17]) == [8, 15, 16]
    assert list(cidx[16]) == [12, 13, 14]
    assert list(cidx[7])  == [3, 4, 5]

    for i in list(range(6)) + list(range(9, 15)):
        assert list(cidx[i]) == []
