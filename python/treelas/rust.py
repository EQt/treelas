"""
Wrapper to the Rust library
"""
import ctypes
from os import path
import numpy as np
from ._treelas.timer import Seconds


_libname = "libtreelars.so"
_libfname = path.join(path.dirname(__file__),
                      "..", "..", "rust", "target", "release", _libname)

if path.exists(_libfname):
    _lib = ctypes.CDLL(_libfname)
    _lib.line_las.restype = ctypes.c_double
    _lib.line_las.argtypes = [
        ctypes.c_size_t,
        ctypes.POINTER(ctypes.c_double),
        ctypes.POINTER(ctypes.c_double),
        ctypes.c_double,
    ]

    def line_las(
            y: np.ndarray,
            lam: float,
            out: np.ndarray = None,
            verbose: bool = False,
            timer: Seconds = Seconds(),
    ) -> np.ndarray:
        y = y.reshape(-1).astype(float)
        if out is None:
            out = np.empty_like(y)
        else:
            assert out.dtype == np.dtype(float)
            assert out.shape == y.shape
        duration = _lib.line_las(
            len(y),
            out.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
            y.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
            lam
        )
        timer.value = duration
        return out
