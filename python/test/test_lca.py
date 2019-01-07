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
    G = nx.DiGraph()
    G.add_edges_from(edges)
