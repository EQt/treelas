from treelas._treelas import _pass_double_pointer as f, Double


def test_pass_pointer():
    assert f() == False
    ptr = Double()
    assert float(ptr) == 0.0
    assert f(ptr) == True
    assert float(ptr) == 123.4
