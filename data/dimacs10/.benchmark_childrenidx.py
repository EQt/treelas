"""
Benchmark ChildrenIndex creation
"""
import h5py
import numpy as np
from graphidx.idx import ChildrenIndex, groupby
from graphidx.py.children import PyChildrenIndex
from graphidx.timer import Timer
from treelas._treelas import _pointer


def benchmark(fname: str):
    with Timer("load hdf5"):
        with h5py.File(fname, "r") as io:
            pi = io["parent"][()].astype(np.int32)

    with Timer("allocate"):
        vals = np.empty_like(pi)
        idx = np.empty(len(pi)+1, dtype=np.int32)
    for a in (idx, vals):
        assert _pointer(a) == a.ctypes.data
        print(" ", hex(a.ctypes.data))

    with Timer("pybind[groupby]", end="\n"):
        groupby(pi, verbose=True, idx=idx, value=vals)
    for a in (idx, vals):
        print(" ", hex(a.ctypes.data))

    with Timer("pybind[ChildrenIdx]", end="\n"):
        cidx = ChildrenIndex(pi, verbose=True)
    print(" root =", cidx.root)
    for d in cidx.__ptr__():
        print(" ", hex(d))

    with Timer("pybind[create CIdx]"):
        ridx = ChildrenIndex([0])

    with Timer("pybind[CIdx.reset]", end="\n"):
        ridx.reset(pi, verbose=True)
    print(" root =", ridx.root)
    for d in ridx.__ptr__():
        print(" ", hex(d))

    with Timer("PyChildrenIndex", end="\n"):
        pidx = PyChildrenIndex.compute(pi)

    return (idx, vals), pidx


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('fname', nargs='*', default=["europe_0.tree"])
    p.add_argument('-c', '--check', action='store_true')
    args = p.parse_args()

    for fname in args.fname:
        print('--', fname)
        (idx, vals), pidx = benchmark(fname)

    if args.check:
        with Timer("check index"):
            assert all(idx == pidx.idx+1)
            assert all(vals[1:] == pidx.pi[:-1])
