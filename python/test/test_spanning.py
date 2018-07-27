import numpy as np
from itertools import combinations
from treelas._treelas import BiAdjacent, random_spanning_tree


def test_clique5():
    edges = zip(*combinations(range(5), 2))
    head, tail = map(lambda l: np.array(l, dtype=np.int32), edges)
    assert (head < tail).all()
    index = BiAdjacent(head, tail)
    # parent = -2*np.ones(5, dtype=np.int32)
    assert list(index[4]) == [0, 1, 2, 3]
    parent = random_spanning_tree(index)
    assert (parent == [1,2,3,4,5]).all(), parent
