#!/usr/bin/env python
"""
Analyze the tree level structure of a tree.

At the moment, BFS is computed using numpy.ndarray.argsort.
However, it should be possible to compute it in linear time if you know the tree (as a linked list, e.g.).
"""
import argparse
import sys
import numpy as np
import matplotlib.pyplot as plt
from numba import njit, jitclass, int64

from .graphviz import print_tree
from .graphio import load_tree
# from queue import Queue


_Queue = [
    ('n',     int64),
    ('first', int64),
    ('last',  int64),
    ('a',     int64[:]),
    ('size',  int64)
]


@jitclass(_Queue)
class Queue(object):
    """Int queue of predefined size"""
    # __slots__ = ['n', 'first', 'last', 'a', 'size']
    def __init__(self, maxsize):
        self.n = maxsize
        self.first = 0
        self.last = 0
        self.a = np.zeros(self.n, dtype=np.int64)
        self.size = 0

    def empty(self):
        return self.size <= 0

    def put(self, x):
        self.a[self.first] = x
        self.first = (self.first + 1) % self.n
        self.size += 1

    def get(self):
        x = self.a[self.last]
        self.last = (self.last + 1) % self.n
        self.size -= 1
        return x


def compute_children(parent):
    """Return V, I such that V[I[j]:I[j+1]] are the children of j"""
    @njit(cache=True)
    def _compute_children(sort, vc, n):
        k = 0
        for i in range(n-1):
            while sort[i+1] >= k:
                vc[k] = i+1
                k += 1

    n = len(parent)
    ci = parent.argsort()
    vc = n * np.ones(n+1, dtype=int)
    sort = parent[ci]
    assert np.all(np.diff(sort) >= 0)
    _compute_children(sort, vc, n)
    return vc, ci


@njit(cache=True)
def compute_bfs(vc, ci, root=0):
    """Return bfs   whereby bfs[i] is the BFS number of i"""
    n = len(ci)
    bfs = -np.ones(n, dtype=np.int64)
    q = Queue(n)
    q.put(root)
    b = 0
    while not q.empty():
        v = q.get()
        bfs[b] = v
        b += 1
        for u in range(vc[v], vc[v+1]):
            q.put(ci[u])
    return bfs


def bfs_order(parent, root=0):
    """BFS in one step"""
    vc, ci = compute_children(parent)
    return compute_bfs(vc, ci, root=root)


def compute_levels(bfs, parent):
    @njit(cache=True)
    def _compute_levels(levels, bfs, parent):
        n = len(parent)
        depth = np.zeros(n, dtype=np.int64)
        d = 0
        levels[0] = 0
        pos = 1
        for i in range(1, n):
            db = 1 + depth[parent[bfs[i]]]
            depth[bfs[i]] = db
            if db > d:
                levels[pos] = i
                pos += 1
                d += 1
        levels[pos] = n
        pos += 1
        return pos

    levels = np.empty_like(parent)
    levels_len = _compute_levels(levels, bfs.astype(np.int64), parent)
    return levels[:levels_len]


def reverse_levels(levels, bfs):
    # @njit
    def _reverse_levels(rev, levels, bfs):
        k = 0
        for i in range(len(levels)-2, -1, -1):
            for j in range(levels[i], levels[i+1]):
                rev[k] = bfs[j]
                k += 1

    rev = np.empty_like(bfs)
    _reverse_levels(rev, levels, bfs)
    return rev


def process(treeh5, debug=False, plot_hist=False, args=None, dfs_order=True):
    root, dfs, parent = load_tree(treeh5)
    if args is not None and args.dot:
        print_tree(parent)

    n = len(dfs)
    bfs = np.zeros(n, dtype=np.int64)
    vc, ci = compute_children(parent)
    bfs = compute_bfs(vc, ci, root=0)
    if debug:
        print(vc[:6])
        print(ci[:6])
        print(bfs[:6])

    order = dfs[::-1] if dfs_order else bfs[::-1]
    rbfs = order
    irbfs = rbfs.argsort()
    access = parent[rbfs]
    access = irbfs[access]
    dacc = np.diff(access)
    if args is not None and args.print_rbfs:
        for i in range(n):
            print(i, ci[vc[i]:vc[i+1]], file=sys.stderr)
        print('   bfs', bfs, file=sys.stderr)
        print('  rbfs', rbfs, file=sys.stderr)
        print(' irbfs', irbfs, file=sys.stderr)
        print('access', access, file=sys.stderr)
    if plot_hist:
        max_hist = 30
        if not dfs_order:
            assert dacc.min() >= 0
            max_hist = dacc.max()
        print('max(access distance) =', dacc.max(), file=sys.stderr)
        print('mean(abs(access distance)) =', np.abs(dacc).mean(),
              file=sys.stderr)
        plt.hist(dacc, max_hist)
    print(dacc.mean(), file=sys.stderr)


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-p', '--plot-hist', action='store_true',
                   help='Plot a histogram')
    p.add_argument('-r', '--print-rbfs', action='store_true',
                   help='Print reversed bfs')
    p.add_argument('-d', '--dot', action='store_true',
                   help='Print tree in dot format')
    p.add_argument('-f', '--dfs-order', action='store_true',
                   help='Do the same but with DFS instead of BFS')
    p.add_argument('treeh5', type=str, nargs='*',
                   help='Tree(s) to process')
    args = p.parse_args()
    for t in args.treeh5:
        print('Processing', t, file=sys.stderr)
        process(t, plot_hist=args.plot_hist, args=args,
                dfs_order=args.dfs_order)
        if args.plot_hist:
            plt.show()
