import treelas as tl


def test_compiler():
    assert hasattr(tl, "__compiler__")
    assert isinstance(tl.__compiler__, str)


def test_author():
    assert hasattr(tl, "__author__")
    assert isinstance(tl.__author__, str)
