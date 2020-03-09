import h5py
from graphidx.idx import ChildrenIndex, groupby
from graphidx.py.children import PyChildrenIndex
from graphidx.timer import Timer


fname = "europe_0.tree"

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


assert all(idx == pidx.idx+1)
assert all(vals[1:] == pidx.pi[:-1])
