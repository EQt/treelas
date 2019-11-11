from treelas._treelas import _pass_double_pointer as f, Seconds


def test_pass_pointer():
    assert f() == False
    ptr = Seconds()
    assert float(ptr) == 0.0
    assert f(ptr) == True
    assert float(ptr) == 123.4
    ptr.value = 42.0
    assert float(ptr) == 42.0
