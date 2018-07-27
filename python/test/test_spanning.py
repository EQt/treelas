import numpy as np
from itertools import combinations
from treelas._treelas import BiAdjacent, random_spanning_tree, find_root


def test_clique5():
    edges = zip(*combinations(range(5), 2))
    head, tail = map(lambda l: np.array(l, dtype=np.int32), edges)
    assert (head < tail).all()

    index = BiAdjacent(head, tail)
    assert list(index[4]) == [0, 1, 2, 3]

    parent = random_spanning_tree(index)
    assert (parent >= 0).all()
    assert (parent < 5).all()
    assert find_root(parent) >= 0
    assert (parent == [3, 4, 1, 4, 4]).all(), parent
    
