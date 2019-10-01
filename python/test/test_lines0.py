import numpy as np
import pytest
from treelas import line_dp
from line import insts

@pytest.mark.parametrize("i", insts)
def test_dual(i):
    res = np.cumsum(i.mu * (i.x - i.y))
    assert np.isclose(res[-1], 0)
    assert all(np.abs(res[:-1]) <= (1 + 1e-9) * i.lam)


@pytest.mark.parametrize("i", insts)
def test_lines(i):
    print(i.mu)
    x = line_dp(y=i.y, lam=i.lam, mu=i.mu)
    assert np.allclose(x, i.x)
