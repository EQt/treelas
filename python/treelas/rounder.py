"""
Utility functions regarding rounding operations
"""


def _int_or_round(x, digits=8):
    """x is an `int` or return `_fround`"""
    if isinstance(x, float) and x.is_integer():
        return int(x)
    return _round(x, digits=digits)


def _round(x, digits=8):
    if hasattr(x, "__round__"):
        return round(x, digits)
    return x


def _fround(x, digits=8) -> str:
    if hasattr(x, "__round__"):
        return f"{round(x, digits):+}"
    return str(x)
