def cxxdemangle(mangled):
    try:
        import cxxfilt

        return cxxfilt.demangle(mangled)
    except ImportError:
        return None


def assert_mangled(mangled, expect):
    demangled = cxxdemangle(mangled)
    if demangled is not None:
        assert expect == demangled


def test_cluster_name():
    """Try to understand, why the bug happens, at the moment:
    - `n1` is defined in cluster.cpp.o
    - `n2` is expected in _treelas.cpython*.so

    The final 'S' marks a substitution [1], i.e. S0_ refers to the
    second identifier (see example in [2]).

    [1]: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangling-compression
    [2]: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#mangle.seq-id

    """

    n1 = "_Z7clusterIi10BiAdjacentE14PartitionIndexIT_EmPKdRKS0_di"
    n2 = "_Z7clusterIi10BiAdjacentE14PartitionIndexIT_EmPKdRKT0_di"

    assert_mangled(n1,
                   "PartitionIndex<int> " +
                   "cluster<int, BiAdjacent>(unsigned long, " +
                   "double const*, BiAdjacent const&, double, int)")

    assert_mangled(n2,
                   "PartitionIndex<int> " +
                   "cluster<int, BiAdjacent>(unsigned long, " +
                   "double const*, BiAdjacent const&, double, int)")
    
    diff = [(i, a, b) for i, (a, b) in enumerate(zip(n1, n2)) if a != b]
    assert diff == [(51, 'S', 'T')]
