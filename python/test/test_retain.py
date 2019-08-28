import numpy as np
import pytest
from treelas import line_las


@pytest.mark.parametrize("lam", [0.3, 1.42, 3.7])
@pytest.mark.parametrize("seed", [42, 1, 2019])
@pytest.mark.parametrize("n", [3, 4, 7, 42])
def test_retain(lam, seed, n):
    """Flow along a line of `n` nodes: Comparing backward and forward flow"""
    np.random.seed(seed)
    y = np.random.randint(6, size=n).astype(float)
    x = line_las(y, lam)
    alpha_f = np.cumsum(x - y)
    alpha_b = np.cumsum(np.flip(x - y))
    assert np.isclose(alpha_b[-1], 0)
    assert np.isclose(alpha_f[-1], 0)
    assert np.allclose(alpha_f + np.flip(alpha_b), x - y)
    assert np.allclose(np.flip(alpha_b)[1:], -alpha_f[:-1])
