import h5py
from distutils.version import LooseVersion
# from pytest import mark


def test_libversion():
    h5v = '.'.join(map(str, h5py.h5.get_libversion()))
    assert h5v >= LooseVersion("1.10")


# @mark.skip
# def test_lowlevel(fname="../../build/test.h5"):
#     from os import path
#
#     if path.exists(fname):
#         file_id = h5py.h5f.open(fname.encode())
#         loc = h5py.h5g.open(file_id, b"blub")
#         assert h5py.h5i.get_name(loc) == b'/blub'
