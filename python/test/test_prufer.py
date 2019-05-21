import numpy as np
from treelas import _treelas
from treelas import Tree


def test_prufer3():
    p = np.array([2, 2, 3], np.int32)
    parent, r = _treelas.prufer2parent(p)
    assert (parent == [2, 2, 3, 4, 4]).all()
    assert r == 4


def test_tree_from_prufer3():
    p = np.array([2, 2, 3], np.int32)
    t = Tree.from_prufer(p)
    assert t.root == 4
    assert (t.parent == [2, 2, 3, 4, 4]).all()


def test_tree_from_prufer5():
    p = [a for i in range(5) for a in [i]*2]
    assert len(p) == 10
    t = Tree.from_prufer(p)
    assert t.n == 12


if __name__ == '__main__':
    import argparse

    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('-s', '--seed', type=int, default=2018,
                   help='Random seed [default=2018]')
    args = p.parse_args()

    children_spec = np.array([2, 2, 3])
    t = Tree.generate(children_spec, seed=args.seed)
    print(f"Generated tree with {t.n} nodes")
    t.show()
