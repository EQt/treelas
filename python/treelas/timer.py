"""
Time a piece of code and print the result (formatted).
"""
import sys
from time import process_time as now
from math import isnan, isinf


class Timer:
    """
    Measure elapsed *process time* using Python's `with` statement,
    i.e. timing for instance `time.sleep(5)` won't report 5 seconds
    but a few milliseconds as the process will be suspended by the
    sleep method.

    >>> def slow(n=100_000):
    ...    i = 0
    ...    for _ in range(n):
    ...       i += 1
    >>> with Timer("call slow"):
    ...    slow()

    """
    verbose = True
    out = sys.stderr

    def __init__(self, msg, fmt='%8.3fms', indent=20, end=''):
        self.fmt = fmt
        self.indent = indent
        self.end = end
        self.msg = msg

    def __float__(self):
        """Duration in seconds"""
        if not hasattr(self, "time0"):
            return float("nan")
        if not hasattr(self, "time1"):
            return float("-inf")
        return self.time1 - self.time0

    def __enter__(self):
        if type(self).verbose:
            if self.end.endswith('\n'):
                print(f"{self.msg}", end=self.end, file=type(self).out)
            else:
                print("%-*s" % (self.indent, self.msg), end=self.end,
                      file=type(self).out, flush=True)
        self.time0 = now()
        return self

    def __exit__(self, *ign):
        self.time1 = now()
        if type(self).verbose:
            if self.end.endswith('\n'):
                out = type(self).out
                print("%-*s" % (self.indent, " ..."), file=out, end='')
            print(self.fmt % (1000.0 * float(self)),
                  file=type(self).out, flush=True)

    def __repr__(self):
        d = float(self)
        if isnan(d):
            return "Timer: not started"
        elif isinf(d):
            return "Timer: running"
        return f"Timer: {d*1000.0:.3f}ms"
