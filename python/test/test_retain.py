import numpy as np
from treelas import line_las


def test_retain(n=10):
    """Flow along a line of `n` nodes: Comparing backward and forward flow"""
    np.random.seed(42)
    y = np.random.randint(6, size=n).astype(float)
    lam = 1
    x = line_las(y, lam)
    alpha_f = np.cumsum(x - y)
    alpha_b = np.cumsum(np.flip(x - y))
    assert np.isclose(alpha_b[-1], 0)
    assert np.isclose(alpha_f[-1], 0)
    assert np.allclose(alpha_f + np.flip(alpha_b), x - y)
