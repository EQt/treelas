import numpy as np
from treelas.idx import PartitionIndex


def test_num5():
    num5 = np.array([1, 2, 0, 0, 2])
    pidx = PartitionIndex(num5)
    assert len(pidx) == 3
    for i in range(3):
        assert set(np.where(num5 == i)[0]) == set(pidx[i]), f"i = {i}"


def test_0001():
    pidx = PartitionIndex([0, 0, 0, 1])
    assert len(pidx) == 2
    assert set(pidx[0]) == {0, 1, 2}
    assert set(pidx[1]) == {3}
