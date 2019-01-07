"""
Determine the lowest common ancestor
"""


def test_defaultdict():
    from collections import defaultdict

    assert bool() == False
    colors = defaultdict(bool)          # dictionary with default False
    assert colors[0] == False
    assert colors[1] == False


def lca_networkx(G, root, pairs):
    """
    [`networkx.algorithms.lowest_common_ancestor`][nx]
    Implemented according to CLRS page 584 (3rd edition).
    Compare to [epp]

    [nx]: https://github.com/networkx/networkx/master/
    [epp]: https://www.ics.uci.edu/~eppstein/PADS/LCA.py

    """
    from collections import defaultdict
    from networkx.utils import UnionFind, arbitrary_element
    from networkx import dfs_postorder_nodes


    pair_dict = defaultdict(set)
    for u, v in pairs:
        pair_dict[u].add(v)
        pair_dict[v].add(u)

    # Iterative implementation of Tarjan's offline lca algorithm
    # as described in CLRS on page 521.
    uf = UnionFind()
    ancestors = {}
    for node in G:
        ancestors[node] = uf[node]

    colors = defaultdict(bool)  
    for node in dfs_postorder_nodes(G, root):
        colors[node] = True
        for v in pair_dict[node]:
            if colors[v]:
                if (v, node) in pairs:
                    yield (v, node), ancestors[uf[v]]
        if node != root:
            parent = arbitrary_element(G.pred[node])
            uf.union(parent, node)
            ancestors[uf[parent]] = parent
