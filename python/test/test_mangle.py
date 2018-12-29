def test_cluster_name():
    """
    Try to understand, why the bug happens, at the moment:
    - `n1` is defined in cluster.cpp.o
    - `n2` is expected in _treelas.cpython*.so
    """

    n1 = "_Z7clusterIi10BiAdjacentE14PartitionIndexIT_EmPKdRKS0_di"
    n2 = "_Z7clusterIi10BiAdjacentE14PartitionIndexIT_EmPKdRKT0_di"

    diff = [(i, a, b) for i, (a, b) in enumerate(zip(n1, n2)) if a != b]
    assert diff == [(51, 'S', 'T')]
