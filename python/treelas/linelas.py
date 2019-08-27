"""
Pure python implementation of the dynamic programming algorithm
on line graphs.

TODO
====

- [ ]. Integrate mu = 0
"""
from __future__ import annotations
import numpy as np
from dataclasses import dataclass
from pprint import pformat
from typing import Optional
from .graphidx.weights import ConstantWeights, Weights
from .rounder import _fround, _int_or_round


@dataclass(repr=False)
class Event:
    x: float
    slope: float

    def __repr__(self):
        s = _int_or_round(self.slope)
        return f"{type(self).__name__}(x={_fround(self.x, 3)}, slope={s})"

    def offset(self) -> float:
        return - self.x * self.slope

    @staticmethod
    def intersect(e1: Event, e2: Event) -> Optional[float]:
        return (e2.offset() - e1.offset()) / (e1.slope - e2.slope)


class DeQue:
    """
    Double ended queue
    """
    def __init__(self):
        from collections import deque
        self._e = deque()

    def pop(self, forward: bool):
        return self._e.popleft() if forward else self._e.pop()

    def peek(self, forward: bool):
        return self._e[-1 if forward else 0]

    def append(self, x, forward: bool):
        if forward:
            self._e.appendleft(x)
        else:
            self._e.append(x)

    def __bool__(self) -> bool:
        return bool(self._e)

    def __len__(self) -> len:
        return len(self._e)

    def __repr__(self) -> str:
        return repr(self._e)


def clip(elem: DeQue[Event], slope: float, offset: float, forward: bool) -> float:
    """
    Clip from little to big indexes (forward) or the other
    way round (not forward)
    """
    dir = 'F' if forward else 'R'
    print(f"clip: ({slope}, {offset}, {dir}): {pformat(elem)}")
    while elem and slope * elem.peek(forward).x + offset < 0:
        e = elem.pop(forward)
        offset += e.offset()
        slope += e.slope
    x = - offset/slope
    elem.append(Event(x, slope), forward)
    return x


def line_lasso(
    y: np.ndarray,
    lam: Weights,
    mu: Weights = ConstantWeights(1.0),
) -> np.ndarray:
    if isinstance(lam, float):
        lam = ConstantWeights(lam)
    n = len(y)
    lb = np.full(n, np.nan)
    ub = np.full(n, np.nan)
    event = DeQue()

    lam0, lam1 = 0.0, lam[0]
    for i in range(n-1):
        lb[i] = clip(event, +mu[i], -mu[i] * y[i] - lam0 + lam1, forward=True)
        ub[i] = clip(event, -mu[i], +mu[i] * y[i] - lam0 + lam1, forward=False)
        lam0, lam1 = lam1, lam[i+1]

    x = np.full(n, np.nan)
    x[-1] = clip(event, mu[-1], -mu[-1] * y[-1] - lam[-1] + 0.0, forward=True)
    for i in range(n-1)[::-1]:
        x[i] = np.clip(x[i+1], lb[i], ub[i])
    return x
