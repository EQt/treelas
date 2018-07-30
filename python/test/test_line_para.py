import numpy as np
from treelas import line_para
from pytest import mark


def test_line1_para():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_para(y, lam)
    assert (x == 0.5).all(), repr(x)


@mark.skip
def test_line2_para():
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_para(y, lam, x)
    assert (x == 0.5).all()


@mark.skip
def test_line3_para():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_para(y, lam, x)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-15).all()
