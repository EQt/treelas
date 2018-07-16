import numpy as np
from treelas import line_condat


def test_condat1():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_condat(y, lam)
    assert (x == 0.5).all()


def test_condat2():
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_condat(y, lam, x)
    assert (x == 0.5).all()


def test_condat3():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_condat(y, lam, x)
    assert (x == [0.9, 0.2, 0.4]).all()
