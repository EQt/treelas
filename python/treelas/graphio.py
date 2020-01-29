"""
Input and output of graph/tree instances.
"""
import h5py
from os import path
from graphidx.tree import Tree


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
    """Return root, parent from a tree instance stored in hdf5"""
    with h5py.File(treeh5, 'r') as io:
        parent = io['parent'][()]
        if parent.max() >= len(parent):
            parent -= 1
        return Tree(parent)


def load_edges(io):
    """Return head, tail from io['edges'] (index starts at zero)"""
    head = io['edges']['head'][()]
    tail = io['edges']['tail'][()]
    if min(head.min(), tail.min()) >= 1:
        head -= 1
        tail -= 1
    return head, tail
