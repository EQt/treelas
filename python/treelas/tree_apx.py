#!/usr/bin/env python
"""
Solve FLSA on trees by computing the derivative at one point per time.

From here, the C++ code for tree12.cpp and tree12c.cpp was developed.
A lot of experimental code concerning orderings is included.
"""
import argparse
import sys
import h5py
import numpy as np
import numba
import multiprocessing

from graphidx.timer import Timer
from graphidx.graphviz import show_tree
from .graphio import load_tree
from .bfs import compute_bfs, compute_children, compute_levels, reverse_levels


PRINT_MAX = 10

float_t = 'f4'
int_t = 'i4'

Node = numba.from_dtype(np.dtype([
    # ('i',    int_t),   # only for debugging
    ('y',      float_t),
    ('x',      float_t),
    ('deriv',  float_t),
    ('parent', int_t)
]))
double = Node.fields['x'][0]
int64 = Node.fields['parent'][0]


def njit(*argp, **args):
    """Enable/Disable caching for all JIT functions"""
    return numba.njit(cache=True, *argp, **args)


def iperm(perm):
    """Return an inverse permutation"""

    @njit
    def iperm_(perm, iperm):
        """Inverse permuation: Implementation"""
        for i, j in enumerate(perm):
            iperm[j] = i
        return iperm

    return iperm_(perm, np.empty_like(perm))


@njit
def init(nodes, y_mid, y, parent, order, iorder):
    for i, ii in enumerate(order):
        nodes[i].y = y[ii]
        nodes[i].x = y_mid
        nodes[i].parent = iorder[parent[ii]]


@njit(locals=dict(x=double, a=double, b=double))
def clip(x, a, b):
    return max(min(x, b), a)


@njit(locals=dict(nodes=Node[:], preorder=int64[:], delta=double,
                  lam=double, mu=double, c=double, d=double))
def discrete_flsa(nodes, preorder, delta, lam, mu=0.5):
    for v in nodes:
        v.deriv = 2.0*mu * (v.x - v.y)

    # compute derivative
    for v in nodes:
        p = nodes[v.parent]
        if abs(p.x - v.x) < delta:
            p.deriv += clip(v.deriv, -lam, +lam)
        elif v.x < p.x:
            #  ==> v.deriv was bigger when v.x == p.x in former iterations
            #  ==> v.deriv was clipped down to +lam (and stays there)
            p.deriv += lam
        else:
            p.deriv -= lam
        # print(' --> % .3f' % p.deriv)

    # optimize root node
    r = nodes[preorder[0]]
    c = 0.5 * delta
    # print("c:", c)
    r.x += c if r.deriv < 0 else -c
    # print("r.i", r.i)

    # backtracing
    for i in preorder[1:]:
        v = nodes[i]
        p = nodes[v.parent]
        if abs(p.x - v.x) <= delta:  # in same range?
            if v.deriv > lam:        # usual clipping
                v.x -= c
            elif v.deriv < -lam:
                v.x += c
            else:
                v.x = p.x
        else:                        # in different ranges
            d = v.deriv + (-lam if v.x < p.x else +lam)
            v.x += c if d < 0 else -c
        # print(' --> % .3f' % v.x)


def discrete_solution(x_opt, x_base, delta):
    """Round optimal continuous solution x_opt to next
       discrete solution  according to x_base + sgn*delta (sgn in [-1, +1])"""
    delta *= 0.5
    x_opt = x_opt.flatten()
    xr = np.empty(x_opt.shape)
    n = len(x_opt)
    for i in range(n):
        xr[i] = x_base[i]
        pos = xr[i] + delta
        neg = xr[i] - delta
        xr[i] = pos if abs(pos - x_opt[i]) < abs(neg - x_opt[i]) else neg
    return xr


def extract_x(nodes, order):
    """Reorder the nodes.x values and return as numpy array"""
    @njit
    def _extract_x(x, nodes, order):
        for i, ii in enumerate(order):
            x[ii] = nodes[i].x
        return x

    return _extract_x(np.empty(len(nodes), dtype=Node.fields['x'][0].name),
                      nodes,
                      order)


def process_tree(treeh5, args=None):
    """
    Load data from `treeh5`, optimize and print the difference to optimum.
    """
    with Timer("Loading Tree"):
        tree = load_tree(treeh5)
        with h5py.File(treeh5, 'r') as io:
            y = io['y'][()]
            lam = io['lam'][()]
            if not isinstance(lam, float):
                lam = float(lam[0])
            xt = io['xt'][()] if 'xt' in io else \
                io['x++'][()] if 'x++' in io else None

    parent = tree.parent
    root = tree.root
    n = len(tree)
    y = y.flatten()
    nodes = np.zeros(n, dtype=Node)
    nodes = np.rec.array(nodes)
    print(f" n={n:,d}".replace(",", "_"))

    if args is not None and args.scale_y:
        y_min, y_max = y.min(), y.max()
        y = y - y_min
        if y_max - y_min > 1e-10:
            y = y / (y_max - y_min)

    y_min, y_max = y.min(), y.max()
    if n == 6:  # for debugging
        y_min, y_max = 0.0, 1.0
    y_mid = 0.5 * (y_min + y_max)
    delta = 0.5 * (y_max - y_min)
    with Timer("Computing Children"):
        vc, ci = compute_children(parent)
    with Timer("Computing BFS"):
        bfs = compute_bfs(vc, ci, root=root)
    with Timer("Reverse BFS"):
        rbfs = bfs.copy()[::-1]
    with h5py.File(treeh5, 'r+') as io:
        if 'bfs' not in io:
            with Timer("Write BFS"):
                io.create_dataset('bfs', data=bfs)
    preorder = bfs.copy()
    levels = None
    if args is not None and args.use_levels:
        with Timer("Computing Levels"):
            levels = compute_levels(bfs, parent)
    if n <= PRINT_MAX:
        if levels is not None:
            print(" levels:", levels)
            for i in range(len(levels)-1):
                print("  %d:" % i, bfs[levels[i]:levels[i+1]])
                print("\nrlevels:", levels[::-1])
                nl = len(levels)
            for i in range(len(levels)-1):
                low = levels[nl-i-2]
                upp = levels[nl-i-1]
                print("  %d [%d:%d):" % (i, low, upp), bfs[low:upp])

    with Timer("Inverse Order"):
        if args is not None and args.use_levels:
            postorder = reverse_levels(levels, bfs)
            backord = iperm(postorder)[reverse_levels(levels, bfs)][::-1]
        else:
            postorder = rbfs.copy()
            backord = np.arange(n)[::-1]
        ipostord = iperm(int64(postorder))

    if 'i' in Node.fields:
        for i, ii in enumerate(postorder):
            nodes[i].i = ii

    if n <= PRINT_MAX:
        vc, ci = compute_children(parent)
        print(" children:", vc)
        print("    cnums:", ci)
        print("     rbfs:", rbfs)
        print(" preorder:", preorder)
        print("postorder:", postorder)
        if levels is not None:
            print(" levelord:", reverse_levels(levels, bfs))
        print("  backord:", backord)
        print(" ipostord:", ipostord)
        print("   iorder:", ipostord[preorder])
        print("identity?:", postorder[ipostord])
        print("        y:", y)
    parent = int64(parent)
    with Timer("Init ...."):
        init(nodes, double(y_mid), double(y), parent, postorder, ipostord)

    if n <= PRINT_MAX:
        # print(nodes)
        print("parent:", parent)
        print("access:", ipostord[parent[postorder]])
        if 'i' in Node.fields:
            print("parent:",
                  np.array([nodes[nodes[i].parent].i for i in postorder]))
        print("    x0:", nodes.x[ipostord])

    with Timer("Iterations:", end="\n"):
        for it in range(args.max_iter):
            print(it+1, '...')
            if n <= PRINT_MAX:
                print("delta:", delta)
                if 'i' in Node.fields:
                    print("nodes.i:", nodes.i)
                    print(" ident?:", nodes.i[ipostord])
                xb = nodes.x[ipostord]
            discrete_flsa(nodes, int64(backord), delta, lam)
            if n <= PRINT_MAX:
                if 'i' in Node.fields:
                    print("    i:", nodes.i[ipostord])
                print("deriv:", nodes.deriv[ipostord])
                print("    y:", nodes.y[ipostord])
                x = nodes.x[ipostord]
                print("    x:", x)
                if xt is not None:
                    sol = discrete_solution(xt, xb, delta)
                    print("  sol:", sol)
                    print(" diff:", sol - x)
                    if np.abs(sol - x).max() >= 1e-7:
                        raise RuntimeError("!!!! INTERNAL ERROR !!!!")
            delta /= 2

    with Timer("Extract solution"):
        x = extract_x(nodes, postorder)
    if n <= PRINT_MAX:
        print("    x:", x)
        if xt is not None:
            print("   xt:", xt.flatten())
    if xt is not None:
        print("Norm(x - xt, Inf): ", np.linalg.norm(x - xt.flatten(), np.inf))
    if args is not None and args.dot_tree:
        with Timer("Show Tree"):
            show_tree(parent)
    if args is not None and args.show_levels:

        def plot():
            import matplotlib.pyplot as plt
            plt.plot(np.diff(levels))
            plt.show()

        multiprocessing.Process(target=plot).start()


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-i', '--max-iter', type=int, default=3,
                   help='Number of iterations')
    p.add_argument('-d', '--dot-tree', action='store_true',
                   help='Show the tree (using graphviz)')
    p.add_argument('-L', '--show-levels', action='store_true',
                   help='Show the distribution of tree levels (depth)')
    p.add_argument('-l', '--use-levels', action='store_true',
                   help='Postorder = reverse Levels(BFS)')
    p.add_argument('-s', '--scale-y', action='store_true',
                   help='Scale y to [0,1]')
    p.add_argument('-v', '--verbose', action='store_true',
                   help='Be more verbose')
    p.add_argument('treeh5', type=str, nargs='*',
                   help='Tree(s) to process')
    args = p.parse_args()
    np.set_printoptions(precision=3)
    for t in args.treeh5:
        print('Processing', t, file=sys.stderr)
        if args.verbose:
            PRINT_MAX = 20
        process_tree(t, args)
