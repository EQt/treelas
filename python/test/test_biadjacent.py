import numpy as np
from treelas.idx import BiAdjacent

def square():
    head = np.array([0, 0, 1, 2])
    tail = np.array([1, 2, 3, 3])
    return BiAdjacent(head, tail)


def test_sqare():
    neigh = square()
    assert repr(neigh) == "BiAdjacent[m = 4, n = 4]"
    assert set(neigh[0]) == {1, 2}
    assert set(neigh[1]) == {0, 3}
    assert set(neigh[2]) == {0, 3}
    assert set(neigh[3]) == {1, 2}


def test_1():
    head = np.array([0, 1, 2, 3], dtype=np.int32)
    tail = np.array([1, 3, 1, 2], dtype=np.int32)
    index = BiAdjacent(head, tail)
    assert repr(index) == "BiAdjacent[m = 4, n = 4]"

    i2 = index[2]
    assert len(i2) == 2

    assert list(i2) == [1, 3]
    assert list(index[0]) == [1]
    assert list(index[1]) == [0, 3, 2]
