from time import sleep
from treelas.timer import Timer, now


def test_timer_not_started():
    t = Timer("")
    assert repr(t) == "Timer: not started"


def test_timer_run1():
    t = None
    with Timer("blub") as t:
        assert t is not None
        assert repr(t) == "Timer: running"
    d = float(t)
    assert repr(t) == f"Timer: {1000.0*d:.3f}ms"
    
