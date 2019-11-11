from treelas._treelas import _pass_double_pointer as f, Seconds
import math


def test_pass_pointer():
    assert f() == False
    ptr = Seconds()
    assert math.isnan(float(ptr))
    assert f(ptr) == True
    assert float(ptr) == 123.4
    ptr.value = 42.0
    assert float(ptr) == 42.0
