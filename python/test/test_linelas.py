import numpy as np
import pytest
from treelas import linelas
from line import insts

linelas.DEBUG = True


@pytest.mark.parametrize("i", insts)
def test_dual(i):
    alpha = np.cumsum(i.mu * (i.x - i.y))
    assert np.isclose(alpha[-1], 0)
    assert all(np.abs(alpha[:-1]) <= i.lam * (1 + 1e-8))


@pytest.mark.parametrize("i", insts)
def test_lines(i):
    x = linelas.line_lasso(y=i.y, lam=i.lam, mu=i.mu)
    assert np.allclose(x, i.x)
