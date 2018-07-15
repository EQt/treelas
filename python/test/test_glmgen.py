import numpy as np
from treelas import line_glmgen


def test_glmgen1():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = line_glmgen(y, lam)
    assert (x == 0.5).all()


def test_glmgen2():
    y = np.array([1, 0, 0.5])
    lam = 0.5
    x = y.copy()
    line_glmgen(y, lam, x)
    assert (x == 0.5).all()
