"""
Benchmark ChildrenIndex creation
"""
import h5py
from graphidx.idx import ChildrenIndex, groupby
from graphidx.py.children import PyChildrenIndex
from graphidx.timer import Timer


def benchmark(fname: str):
    with Timer("load hdf5"):
        with h5py.File(fname) as io:
            pi = io["parent"][()]

    with Timer("pybind", end="\n"):
        (idx, vals) = groupby(pi, verbose=True)

    with Timer("pybind", end="\n"):
        cidx = ChildrenIndex(pi, verbose=True)

    with Timer("pybind", end="\n"):
        cidx2 = ChildrenIndex([0]).reset(pi, verbose=True)

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
