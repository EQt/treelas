from numba import njit


def compute_njit(a, b):
    @njit(cache=True)
    def _compute(a, b):
        return a + b

    return  _compute(a,b)


def test1():
    """Does numba.njit emit a warning?"""
    assert compute_njit(5, 8) == 13
