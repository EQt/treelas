module TestMST
using Test

include("../src/mst.jl")


function from_networkx(graph)
    graph = reshape(reinterpret(Int, graph), 3, length(graph))'
    edges = vec(mapslices(Tuple, graph[:, 1:2] .+ 1, dims = [2]))
    weights = Float64.(graph[:, 3])
    n = maximum(map(maximum, edges))
    return n, edges, weights
end


@testset "MST: Kruskal Wikipedia         " begin
    # Adapted from
    # https://github.com/networkx/networkx/blob/master/
    #   networkx/algorithms/tree/tests/test_mst.py
    graph_x = [(0, 1, 7),
               (0, 3, 5),
               (1, 2, 8),
               (1, 3, 9),
               (1, 4, 7),
               (2, 4, 5),
               (3, 4, 15),
               (3, 5, 6),
               (4, 5, 8),
               (4, 6, 9),
               (5, 6, 11)]
    n, edges, weights = from_networkx(graph_x)

    selected = kruskal_mst(n, edges, weights)
    expected_x = [(0, 1, 7),
                  (0, 3, 5),
                  (1, 4, 7),
                  (2, 4, 5),
                  (3, 5, 6),
                  (4, 6, 9)]
    expected = [e in expected_x for e in graph_x]
    # println(Int.(selected .⊻ expected))
    @test selected == expected

    selected = kruskal_mst(n, edges, -weights)
    expected_x = [(0, 1, 7),
                  (1, 2, 8),
                  (1, 3, 9),
                  (3, 4, 15),
                  (4, 6, 9),
                  (5, 6, 11)]
    expected = [e in expected_x for e in graph_x]
    @test selected == expected
end

end
