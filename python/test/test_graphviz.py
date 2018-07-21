from treelas.graphviz import print_dot, print_tree
from os import path
import h5py
from io import StringIO


mini_dot =  """digraph tree {
   rankdir = BT
   0 -> 1
   3 -> 1
   4 -> 2
   5 -> 3
}
"""


def test_print_tree():
    fname = path.join(path.dirname(__file__), '..', '..', 'data', 'tree.mini.h5')
    with h5py.File(fname) as io:
        parent = io['parent'][:]
    buf = StringIO()
    print_tree(parent, out=buf)
    assert buf.getvalue() == mini_dot
