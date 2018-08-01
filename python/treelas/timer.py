"""
Time a piece of code and print the result (formatted).
"""
import sys
from time import process_time as now
from math import isnan, isinf


class Timer:
    """Measure elapsed process time using Python's `with` statement"""
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
                      file=type(self).out)
            type(self).out.flush()
        self.time0 = now()
        return self

    def __exit__(self, *ign):
        self.time1 = now()
        if type(self).verbose:
            if self.end.endswith('\n'):
                print("%-*s" % (self.indent, " ..."), file=type(self).out, end='')
            print(self.fmt % (1000.0 * float(self)),
                  file=type(self).out)
            type(self).out.flush()
        return self

    def __repr__(self):
        d = float(self)
        if isnan(d):
            return "Timer: not started"
        elif isinf(d):
            return "Timer: running"
        return f"Timer: {d*1000.0:.3f}ms"
