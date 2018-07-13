import h5py
from pytest import mark
from distutils.version import LooseVersion

@mark.xfail
def test_libversion():
    h5v = '.'.join(map(str, h5py.h5.get_libversion()))
    assert h5v <= LooseVersion("1.8")
