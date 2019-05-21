from treelas.graphviz import print_tree
from os import path
import h5py
from io import StringIO


mini_dot =  """digraph tree {
   rankdir = BT
   1 -> 0
   2 -> 1
   3 -> 0
   4 -> 1
   5 -> 2
}
"""


def test_print_tree():
    fname = path.join(path.dirname(__file__), '..', '..', 'data', 'tree.mini.h5')
    with h5py.File(fname) as io:
        parent = io['parent'][:]
    buf = StringIO()
    print_tree(parent, out=buf)
    assert buf.getvalue() == mini_dot


def test_import():
    from treelas.graphviz import print_dot
    assert print_dot
