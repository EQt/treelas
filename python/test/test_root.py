import numpy as np
import treelas as tl


def test_root_4():
    parent = np.array([1, 1, 2, 3], dtype=np.int32)
    assert tl.find_root(parent) == 1


def test_root_3():
    parent = np.array([1, 2, 3, 3], dtype=np.int32)
    assert tl.find_root(parent) == 3
