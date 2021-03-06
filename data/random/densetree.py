#!/usr/bin/env python
"""
Generate random dense trees.
"""
import numpy as np
import sys
import treelas as tl


FACTOR = 1e-5
DISTS = {
    "norm": lambda n: np.random.normal(scale=factor*n, loc=0.5*n, size=n-2),
    "exp": lambda n: np.random.exponential(scale=factor*n, size=n-2),
    "mix": lambda n: np.random.randint(0, n, size=n-2) + \
                     np.abs(np.random.normal(size=n-2, scale=0.01*n)),
}


def plot_degrees(degs, exclude_leafs=True):
    if exclude_leafs:
        degs = degs[degs > 1]
    bins = min(degs.max()+1, 100)
    plt.hist(degs, bins=bins, log=True, density=False)


def generate(n, seed=2020, dist="norm"):
    np.random.seed(seed)
    p = DISTS[dist](n)
    t = tl.Tree.from_prufer(p.astype(int).clip(0, n-1))
    deg = t.degree
    nchild = deg[deg >= 2] - 1
    assert abs(deg.mean() - 2*(n-1)/n) < 1e-12, f'deg={deg.mean()}'
    print(f"n = {n:,d}".replace(",", "_"))
    print(f"#child: mean={nchild.mean():.3} #std={nchild.std():.5}", end=' ')
    print(f"deg.std={deg.std():.5}")
    return t


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-t', '--show-tree', action='store_true',
                   help='Plot the tree using graphviz')
    p.add_argument('-p', '--plot-deg', action='store_true',
                   help='Plot degree distribution')
    p.add_argument('-n', '--num-nodes', type=float, default=2**20)
    p.add_argument('-d', '--distribution', type=str, default='norm')
    p.add_argument('-s', '--seed', type=int, default=2020)
    p.add_argument('-5', '--out-h5', type=str, default=None,
                   help='Store Tree in HDF5 and exit')
    p.add_argument('-f', '--factor', type=float, default=1e-5,
                   help='Scaling factor')

    args = p.parse_args()
    factor = args.factor

    t = None
    if args.show_tree:
        from random import randint as ri

        t = generate(int(args.num_nodes),
                     seed=args.seed,
                     dist=args.distribution)
        n = len(t)

        nas = ['node [margin=0, width=0.5,shape=circle,label="",style=filled]'] + \
            [f'{i} [fillcolor="/rdylbu9/{ri(3,6)}"]' for i in range(n)]
        t.show(node_attrs=nas, out=sys.stdout)

    if args.out_h5 or args.plot_deg:
        t = generate(int(args.num_nodes),
                     seed=args.seed,
                     dist=args.distribution,
                     factor=args.factor)
    if args.out_h5:
        t.save_h5(args.out_h5)

    if args.plot_deg:
        import matplotlib.pyplot as plt

        if t:
            plot_degrees(t.degree)
        else:
            for e in [19, 20, 21, 23]:
                plt.figure(f'generate(n={2**e:,d})')
                t = generate(2**e, dist=args.distribution)
                plot_degrees(t.degree)
        plt.show()
