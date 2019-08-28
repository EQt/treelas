from treelas.linelas import DeQue


def test_empty_bool():
    q = DeQue()
    assert not q
    assert len(q) == 0


def test_one_push():
    q = DeQue()
    q.push(42)
    assert q
    assert len(q) == 1
    assert q.peek() == 42
    assert repr(q) == "DeQue([42])"

def test_one_push_back():
    q = DeQue()
    q.push(42, False)
    assert q
    assert len(q) == 1
    assert q.peek() == 42
    assert q.peek(False) == 42


def test_fbf():
    q = DeQue()
    q.push(1)
    q.push(5, False)
    q.push(2)
    assert list(q) == [2, 1, 5]
    assert repr(q) == "DeQue([2, 1, 5])"


def test_fbfBB():
    q = DeQue()
    q.push(1)
    q.push(5, False)
    q.push(2)
    assert list(q) == [2, 1, 5]
    assert q.peek() == 2
    assert q.peek(False) == 5
    assert q.pop(False) == 5
    assert list(q) == [2, 1]
    assert q.peek(False) == 1
    assert q.pop(False) == 1
    assert q
    assert q.peek(False) == 2
    assert q.peek() == 2
