import numpy as np
import pytest
from treelas import line_las
from line import insts


@pytest.mark.xfail(reason="not ready")
@pytest.mark.parametrize("i", insts)
def test_lines(i):
    x = line_las(y=i.y, lam=i.lam, mu=i.mu)
    assert np.allclose(x, i.x)
