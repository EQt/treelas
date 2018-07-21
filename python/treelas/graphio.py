"""
Input and output of graph/tree instances.
"""
import h5py
from os import path


def njoin(*pathel):
    """Return normed path constructed out of *pathels, relative to this dir"""
    if len(pathel) == 1 and isinstance(pathel[0], list):
        return njoin(*pathel[0])
    if len(pathel) == 1 and isinstance(pathel[0], str):
        if '/' in pathel[0]:
            return njoin(*pathel[0].split('/'))
    __dir__ = path.dirname(__file__)
    return path.normpath(path.join(__dir__, *pathel))


def load_tree(treeh5):
    """Return root, dfs, parent from a tree instance stored in hdf5"""
    with h5py.File(treeh5, 'r') as io:
        dfs = io['dfs'].value
        root = dfs[0]
        parent = io['parent'].value
        if dfs.min() >= 1:
            parent -= 1
            root -= 1
            dfs -= 1
            assert parent[root] == root
        return root, dfs, parent


def load_edges(io):
    """Return head, tail from io['edges'] (index starts at zero)"""
    head = io['edges']['head'].value
    tail = io['edges']['tail'].value
    if min(head.min(), tail.min()) >= 1:
        head -= 1
        tail -= 1
    return head, tail


# Mini test instance
TREE_MINI = njoin(*('../data/tree/tree.mini.h5'.split('/')))
