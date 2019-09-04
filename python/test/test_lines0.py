import numpy as np
import pytest
from treelas import line_dp
from line import insts


@pytest.mark.parametrize("i", insts)
def test_lines(i):
    print(i.mu)
    x = line_dp(y=i.y, lam=i.lam, mu=i.mu)
    assert np.allclose(x, i.x)
