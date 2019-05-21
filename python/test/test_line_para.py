import numpy as np
from treelas import line_para


def test_line1_para():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_para(y, lam)
    assert (x == 0.5).all(), repr(x)


def test_line2_para():
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_para(y, lam, x)
    assert (x == 0.5).all()


def test_line3_para():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_para(y, lam, x)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-15).all()


def test_line3_para1():
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_para(y, lam, x, parallel=True)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-15).all()


def test_line4_para():
    y = np.array([-0.28,  0.58,  2.15, -1.28])
    lam = 0.01
    x = line_para(y, lam)
    expected = np.array([-0.27,  0.58,  2.13, -1.27])
    assert (np.abs(x - expected) < 1e-15).all(), x


def test_line4_para1():
    y = np.array([-0.28,  0.58,  2.15, -1.28])
    lam = 0.01
    x = line_para(y, lam, parallel=True)
    expected = np.array([-0.27,  0.58,  2.13, -1.27])
    assert (np.abs(x - expected) < 1e-15).all(), x
