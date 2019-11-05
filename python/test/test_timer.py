from treelas._treelas import timer
import treelas._treelas as tl


def test_timer():
    assert tl._pass_timer_ref() <= 0
    with timer.TimerQuiet():
        tim = timer.Timer()
        d = tl._pass_timer_ref(tim)
        assert d >= 0.05
        assert d <= 0.05 * 1.01
        assert d == float(tim)
