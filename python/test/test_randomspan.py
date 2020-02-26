import os
import pytest
import numpy as np
from graphidx import GridGraph
from graphidx.idx import BiAdjacent
from graphidx import random_spanning_tree
from graphidx.tree import Tree
from treelas import tree_dp


def test_grid_2x2():
    graph = BiAdjacent(GridGraph(2, 2))
    pi = random_spanning_tree(graph, seed=0)
    if os.getenv("show"):
        Tree(pi).show()
    np.random.seed(0)
    y = np.random.randn(len(pi))
    x = tree_dp(y=y, parent=pi, lam=0.2)


def test_grid_3x3():
    graph = BiAdjacent(GridGraph(3, 3))
    pi = random_spanning_tree(graph, seed=0)
    if os.getenv("show"):
        Tree(pi).show()
    assert all(pi == [3, 0, 1, 6, 7, 8, 6, 6, 7])
    np.random.seed(0)
    y = np.random.randn(len(pi))
    x = tree_dp(y=y, parent=pi, lam=0.2)


@pytest.mark.parametrize("width", [3, 10, 50, 100, 200])
def test_grid(width):
    graph = BiAdjacent(GridGraph(width, width))
    pi = random_spanning_tree(graph, seed=0)
    np.random.seed(0)
    y = np.random.randn(len(pi))
    x = tree_dp(y=y, parent=pi, lam=0.2)
