from treelas.lca import lca_networkx


def test_networkx1():
    """
         0
        / \
       1   2
      /|   |\
     3 4   5 6
    """
    import networkx as nx

    edges = [(0, 1), (0, 2), (1, 3), (1, 4), (2, 5), (2, 6)]
    root = 0
    G = nx.DiGraph()
    G.add_edges_from(edges)

    [lca_34] = lca_networkx(G, root, [(3, 4)])
    assert lca_34 == ((3, 4), 1)

    [lca_35] = lca_networkx(G, root, [(3, 5)])
    assert lca_35 == ((3, 5), 0)

    [lca_31] = lca_networkx(G, root, [(3, 1)])
    assert lca_31 == ((3, 1), 1)

    [lca_16] = lca_networkx(G, root, [(1, 6)])
    assert lca_16 == ((1, 6), 0)

    try:
        [lca_61] = lca_networkx(G, root, [(6, 1)])
    except ValueError:
        pass
