import pytest
import numpy as np
from treelas import TreeInstance
from data import test_dir


@pytest.fixture
def tree0():
    return TreeInstance.load(test_dir("tree0.toml"))


@pytest.fixture
def tree1():
    return TreeInstance.load(test_dir("tree0.1.toml"))


def test_repr0(tree0):
    assert repr(tree0).startswith("TreeInstance(root=35,\n")


def test_repr1(tree1):
    assert repr(tree1).startswith("TreeInstance(root=17,\n")


def test_solve_len0(tree0):
    x = tree0.solve().x
    assert len(x) == len(tree0)
    assert len(x) == 36


def test_solve_len1(tree1):
    x = tree1.solve().x
    assert len(x) == len(tree1)
    assert len(x) == 18


def test_solve_x(tree0):
    ti = tree0
    x = ti.solve().x
    print(x)
    assert np.allclose(x,
        [0.79389311] * 27
      + [0.57753449] * 3
      + [0.57439977] * 3
      + [0.57753449, 0.57439977, 0.57753449]
    )


def test_gap_x(tree0):
    pass
