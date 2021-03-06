"""
Pure python implementation of the dynamic programming algorithm
on line graphs.
"""
from __future__ import annotations
import numpy as np
from dataclasses import dataclass
from pprint import pformat as _fmt
from typing import Optional
from graphidx.weights import Ones, Weights
from .rounder import _fround, _int_or_round
from .queue import DeQue


DEBUG = False
EPS = 1e-10


@dataclass(repr=False, init=False)
class Event:
    x: float
    slope: float

    def __init__(self, x: float, slope: float, offset=None):
        self.x = x
        self.slope = slope

    def __repr__(self):
        s = _int_or_round(self.slope)
        return f"{type(self).__name__}(x={_fround(self.x, 3)}, slope={s:+}" + \
            f", offset={_fround(self.offset())})"

    def offset(self) -> float:
        return - self.x * self.slope

    @staticmethod
    def intersect(e1: Event, e2: Event) -> Optional[float]:
        return (e2.offset() - e1.offset()) / (e1.slope - e2.slope)


def clip(elem: DeQue[Event], slope: float, offset: float, forward: bool) -> float:
    """
    Clip from little to big indexes (forward) or the other
    way round (not forward)
    """
    if DEBUG:
        print(f"clip_{'f' if forward else 'r'}:",
              f"({slope:+}, {offset:+}):\n{_fmt(elem._e)}")
        if elem:
            print(' test:', _fround(slope * elem.peek(forward).x + offset))
        else:
            print('empty')
    while elem and slope * elem.peek(forward).x + offset < 0:
        e = elem.pop(forward)
        offset += e.offset()
        slope += e.slope
        if DEBUG and elem:
            print(' test:', _fround(slope * elem.peek(forward).x + offset))
        else:
            print('empty')
    if abs(slope) <= EPS:
        x = -np.inf if forward else +np.inf
    else:
        x = - offset/slope
        elem.push(Event(x, slope), forward)
    if DEBUG:
        print(f" --> x = {_fround(x)}")
    return x


def line_lasso(
    y: np.ndarray,
    lam: Weights,
    mu: Weights = Ones(),
) -> np.ndarray:
    """
    Return an minimizer `x` of the fused lasso problem on line graph
    with node values `y` (weighted with `mu`) and edge weights `lam`.
    """
    lam = Weights.new(lam)
    mu = Weights.new(mu)
    n = len(y)
    lb = np.full(n, np.nan)
    ub = np.full(n, np.nan)
    event = DeQue(n)

    lam0 = 0.0          # old lambda
    for i in range(n-1):
        lb[i] = clip(event, +mu[i], -mu[i] * y[i] - lam0 + lam[i], True)
        ub[i] = clip(event, -mu[i], +mu[i] * y[i] - lam0 + lam[i], False)
        lam0 = lam[i] if mu[i] > EPS else min(lam0, lam[i])
        if DEBUG:
            print("lam0:", lam0, '( lam[i] =', lam[i], ')\n')

    x = np.full(n, np.nan)
    x[-1] = clip(event, mu[-1], -mu[-1] * y[-1] - lam0 + 0.0, forward=True)
    for i in range(n-1)[::-1]:
        if DEBUG:
            print(lb[i], ub[i])
        x[i] = np.clip(x[i+1], lb[i], ub[i])
    return x


if __name__ == '__main__':
    # mini test instance
    DEBUG = True
    line_lasso(y=[1, 2, 1], lam=0.1)
