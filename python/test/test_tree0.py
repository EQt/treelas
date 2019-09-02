import pytest
import numpy as np
from treelas import TreeInstance
from data import test_dir


@pytest.fixture
def tree0():
    return TreeInstance.load(test_dir("tree0.toml"))


def test_repr(tree0):
    ti = tree0
    assert repr(ti).startswith("TreeInstance(root=35,\n")


def test_solve_len(tree0):
    ti = tree0
    x = ti.solve().x
    assert len(x) == len(ti)
    assert len(x) == 36


def test_solve_x(tree0):
    ti = tree0
    x = ti.solve().x
    print(x)
    assert np.allclose(x,
        [0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311,
         0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311,
         0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311,
         0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311, 0.79389311,
         0.79389311, 0.79389311, 0.79389311, 0.57753449, 0.57753449, 0.57753449,
         0.57439977, 0.57439977, 0.57439977, 0.57753449, 0.57439977, 0.57753449]
    )
