import pytest
numba = pytest.importorskip("numba")


def compute_njit(a, b):
    @numba.njit(cache=True)
    def _compute(a, b):
        return a + b

    return  _compute(a,b)


def test1():
    """Does numba.njit emit a warning?"""
    assert compute_njit(5, 8) == 13
