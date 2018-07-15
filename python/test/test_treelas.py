"""
Very basic tests concerning refcounts and boost::numpy
"""
import treelas as t
import numpy as np
import sys
import gc


def test_create_array():
    a = t._treelas._test_create_array()
    assert a.dtype == np.float64
    assert len(a) == 3
    assert (a == np.array([13, -1, 42.])).all()


def test_refcount():
    a = t._treelas._test_create_array()
    # generally one higher than expected --> see sys.getrefcount.__doc__
    assert sys.getrefcount(a) <= 2
