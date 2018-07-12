import h5py
from distutils.version import LooseVersion


def test_libversion():
    h5v = '.'.join(map(str, h5py.h5.get_libversion()))
    assert h5v <= LooseVersion("1.8")
