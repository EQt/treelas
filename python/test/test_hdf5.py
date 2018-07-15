import h5pyfrom pytest import mark
from distutils.version import LooseVersion

@mark.xfail
def test_libversion():
    h5v = '.'.join(map(str, h5py.h5.get_libversion()))
    assert h5v <= LooseVersion("1.8")


def test_lowlevel():
    file_id = h5py.h5f.open("../../build/test.h5".encode())
    loc = h5py.h5g.open(file_id, "blub".encode())
    assert h5py.h5i.get_name(loc) == b'/blub'
