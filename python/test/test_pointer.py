from treelas._treelas import _pass_double_pointer as f
import ctypes


def test_pass_pointer():
    # assert f() == False
    ptr = ctypes.c_double()
    assert ptr.value == 0.0
    assert f(ptr) == True
