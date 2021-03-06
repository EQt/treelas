"""
Very basic tests concerning refcounts and boost::numpy
"""
import treelas as t
import numpy as np
import sys


def test_create_array():
    a = t._treelas._test_create_array()
    assert a.dtype == np.float64
    assert len(a) == 3
    assert a.flags.writeable
    assert a.flags.owndata
    assert a.flags.c_contiguous
    assert a.flags.aligned
    assert (a == np.array([13, -1, 42.])).all()


def test_empty_array_f64():
    a = t._treelas._empty_array_f64()
    assert a.dtype == np.float64
    assert a.shape == (0,)


def test_empty_array_i32():
    a = t._treelas._empty_array_i32()
    assert a.dtype == np.int32
    assert a.shape == (0,)


def test_is_empty():
    assert t._treelas._is_empty(np.array([]))
    assert t._treelas._is_empty(np.array([], dtype=np.int32))
    assert t._treelas._is_empty(t._treelas._empty_array_f64())
    assert t._treelas._is_empty(t._treelas._empty_array_i32())


def test_refcount():
    a = t._treelas._test_create_array()
    # generally one higher than expected --> see sys.getrefcount.__doc__
    assert sys.getrefcount(a) <= 2
