import numpy as np
from treelas.idx import PartitionIndex


def test_num5():
    num5 = np.array([1, 2, 0, 0, 2])
    pidx = PartitionIndex(num5)

    for i in range(3):
        assert set(np.where(num5 == i)[0]) == set(pidx[i]), f"i = {i}"
