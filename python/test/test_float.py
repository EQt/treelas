class X:
    """How to overload float(X)?"""
    def __init__(self, val=0.42):
        self.val = val

    def __repr__(self):
        return f"{type(self).__name__}(val={self.val})"

    def __float__(self):
        return self.val


def test_float1():
    x = X()
    assert repr(x) == "X(val=0.42)"
    assert float(x) == 0.42
    
