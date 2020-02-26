"""
Compute an average tree solution with random spanning trees
"""
import h5py
import numpy as np
from os import path

import treelas
from graphidx import random_spanning_tree
from graphidx import GridGraph
from graphidx.idx import BiAdjacent


def average_tree(fname: str, lam: float, rep: int):
    with h5py.File(fname) as io:
        y = io["y"][()]

    grid_opt = fname.replace(".img", f"_lam{lam}.grid_opt")
    if path.exists(grid_opt):
        with h5py.File(grid_opt) as io:
            print(grid_opt)
            grid_opt = io["xgrid"][()]
    else:
        grid_opt = None
    graph = BiAdjacent(GridGraph(*y.shape))
    yvec = y.reshape(-1, order='F')
    xsol = list()
    for s in range(rep):
        print("seed =", s)
        pi = random_spanning_tree(graph, seed=s)
        assert len(pi) == len(yvec)
        assert pi.max() < len(pi)
        assert pi.min() >= 0
        x = treelas.tree_dp(y=yvec, parent=pi, lam=8*lam, verbose=False)
        xsol.append(x.reshape(*y.shape, order='F'))

    xopt = np.mean(xsol, axis=0)
    return xsol, xopt, grid_opt


if __name__ == '__main__':
    import matplotlib.pyplot as plt
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("fname", nargs='?', default="phantom_w200.img")
    p.add_argument("-r", "--rep", type=int, default=50)
    p.add_argument("-l", "--lam", type=float, default=0.2)
    args = p.parse_args()

    xsol, xopt, grid_opt = average_tree(args.fname, args.lam, args.rep)

    for i, x in enumerate(xsol[:5]):
        plt.figure(f"seed={i}")
        plt.imshow(x, vmin=0, vmax=1)
    plt.figure('xopt')
    plt.imshow(xopt, vmin=0, vmax=1)
    if grid_opt is not None:
        plt.figure('xgrid')
        plt.imshow(grid_opt, vmin=0, vmax=1)
    else:
        print("grid_opt not found")
    plt.show()
