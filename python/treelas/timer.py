"""
Metaprogramming stuff like Timer, __line__ method, etc.
"""
import sys
from time import process_time as now


class Timer:
    """Measure elapsed process time using Python's `with` statement"""
    verbose = True
    out = sys.stderr

    def __init__(self, msg, fmt='%8.3fms', indent=20, end=''):
        self.fmt = fmt
        self.indent = indent
        self.end = end
        self.msg = msg

    def __enter__(self):
        if type(self).verbose:
            if self.end.endswith('\n'):
                print(f"{self.msg}", end=self.end, file=type(self).out)
            else:
                print("%-*s" % (self.indent, self.msg), end=self.end,
                      file=type(self).out)
            type(self).out.flush()
            self.time0 = now()

    def __exit__(self, *ign):
        if type(self).verbose:
            time1 = now()
            if self.end.endswith('\n'):
                print("%-*s" % (self.indent, " ..."), file=type(self).out, end='')
            print(self.fmt % (1000.0 * (time1 - self.time0)),
                  file=type(self).out)
            type(self).out.flush()
