import pytest
import numpy as np
from treelas import line_lasc, line_las2, line_las3


LINE_IMPL = [line_lasc, line_las2, line_las3]


@pytest.mark.parametrize("line_lasc", LINE_IMPL)
def test_cline1(line_lasc):
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_lasc(y, lam)
    assert (x == 0.5).all(), repr(x)


@pytest.mark.parametrize("line_lasc", LINE_IMPL)
def test_cline2(line_lasc):
    """with pre-allocated output array"""
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_lasc(y, lam, x)
    assert (x == 0.5).all()


@pytest.mark.parametrize("line_lasc", LINE_IMPL)
def test_cline3(line_lasc):
    """different lambda"""
    y = np.array([1, 0, 0.5])
    lam = 0.1
    x = y.copy()
    line_lasc(y, lam, x)
    expected = np.array([0.9, 0.2, 0.4])
    assert (np.abs(x - expected) < 1e-15).all()
