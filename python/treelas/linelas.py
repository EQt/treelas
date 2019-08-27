"""
Pure python implementation of the dynamic programming algorithm
on line graphs.

TODO
====

- [ ]. Integrate mu = 0
"""
from __future__ import annotations
import numpy as np
from collections import deque
from dataclasses import dataclass
from pprint import pformat
from typing import Optional
from .graphidx.weights import ConstantWeights
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


def clip(elements: deque, slope: float, offset: float, forward: bool) -> float:
    dir = 'F' if forward else 'R'
    first = -1 if forward else 0
    print(f"clip: ({slope}, {offset}, {dir}): {pformat(elements)}")
    while elements and slope * elements[first].x + offset < 0:
        e = elements.popleft() if forward else elements.pop()
        offset += e.offset()
        slope += e.slope
    x = - offset/slope
    if forward:
        elements.appendleft(Event(x, slope))
    else:
        elements.append(Event(x, slope))
    return x


def line_lasso(y: np.ndarray, lam: float, mu = ConstantWeights(1.0)):
    n = len(y)
    lb = np.full(n, np.nan)
    ub = np.full(n, np.nan)
    event = deque()

    lam0, lam1 = 0.0, lam
    for i in range(n-1):
        lb[i] = clip(event, +mu[i], -mu[i] * y[i] - lam0 + lam1, forward=True)
        ub[i] = clip(event, -mu[i], +mu[i] * y[i] - lam0 + lam1, forward=False)
        lam0, lam1 = lam1, lam

    x = np.full(n, np.nan)
    x[-1] = clip(event, mu[-1], -mu[-1] * y[-1] - lam + 0.0, forward=True)
    for i in range(n-1)[::-1]:
        x[i] = np.clip(x[i+1], lb[i], ub[i])
    return x
