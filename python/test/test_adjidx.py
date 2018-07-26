import numpy as np
from treelas.adjidx import BiAdjecent


def test_1():
    head = np.array([0, 1, 2, 3], dtype=np.int32)
    tail = np.array([1, 3, 1, 2], dtype=np.int32)
    index = BiAdjecent(head, tail)
    assert repr(index) == "BiAdjacent[m = 4, n = 4]"

    i2 = index[2]
    assert len(i2) == 2

    assert list(i2) == [1, 3]
    assert list(index[0]) == [1]
    assert list(index[1]) == [0, 3, 2]
