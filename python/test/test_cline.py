import numpy as np
from treelas import line_lasc


def test_cline1():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_lasc(y, lam)
    assert (x == 0.5).all(), repr(x)


def test_cline2():
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_lasc(y, lam, x)
    assert (x == 0.5).all()


def test_cline3():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_lasc(y, lam, x)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-15).all()
