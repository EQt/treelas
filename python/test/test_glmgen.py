import numpy as np
from treelas import line_glmgen


def test_glmgen1():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_glmgen(y, lam)
    assert (x == 0.5).all()


def test_glmgen2():
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_glmgen(y, lam, x)
    assert (x == 0.5).all()


def test_glmgen3():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_glmgen(y, lam, x)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-16).all()
