import numpy as np
from itertools import combinations
from treelas._treelas import BiAdjacent, random_spanning_tree


def test_clique5():
    edges = zip(*combinations(range(5), 2))
    head, tail = map(lambda l: np.array(l, dtype=np.int32), edges)
    index = BiAdjacent(head, tail)
    parent = random_spanning_tree(index)
