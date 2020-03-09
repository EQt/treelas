import h5py
from graphidx.idx import ChildrenIndex, groupby
from graphidx.timer import Timer


fname = "europe_0.tree"
with h5py.File(fname) as io:
    pi = io["parent"][()]


with Timer("pybind", end="\n"):
    gidx = groupby(pi, verbose=True)

with Timer("pybind", end="\n"):
    cidx = ChildrenIndex(pi, verbose=True)

with Timer("pybind", end="\n"):
    cidx2 = ChildrenIndex([0]).reset(pi, verbose=True)
