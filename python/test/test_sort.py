import numpy as np
from treelas._treelas import _stdcxx_sort as cxx_sort


def test_sort(n=1024):
    a = np.random.uniform(size=n)
    c = a.copy()
    b = np.sort(a)
    cxx_sort(c)
    assert (c == b).all()


