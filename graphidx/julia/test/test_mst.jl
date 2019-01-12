module TestMST
using Test

include("../src/mst.jl")


function from_networkx(graph; print_it = false)
    graph = reshape(reinterpret(Int, graph), 3, length(graph))'
    if print_it
        for i in 1:size(graph, 1)
            e = graph[i, :]
            println("$('A' + e[1]) -- $('A' + e[2]): $(e[3])")
        end
    end
    edges = vec(mapslices(Tuple, graph[:, 1:2] .+ 1, dims = [2]))
    weights = Float64.(graph[:, 3])
    n = maximum(map(maximum, edges))
    return n, edges, weights
end


# Adapted from
# https://github.com/networkx/networkx/blob/master/
#   networkx/algorithms/tree/tests/test_mst.py
const graph_x = [
    (0, 1, 7),
    (0, 3, 5),
    (1, 2, 8),
    (1, 3, 9),
    (1, 4, 7),
    (2, 4, 5),
    (3, 4, 15),
    (3, 5, 6),
    (4, 5, 8),
    (4, 6, 9),
    (5, 6, 11),
]

const expected_min_x = [
    (0, 1, 7),
    (0, 3, 5),
    (1, 4, 7),
    (2, 4, 5),
    (3, 5, 6),
    (4, 6, 9),
]

const expected_max_x = [
    (0, 1, 7),
    (1, 2, 8),
    (1, 3, 9),
    (3, 4, 15),
    (4, 6, 9),
    (5, 6, 11),
]


@testset "MST: Kruskal Wikipedia         " begin
    n, edges, weights = from_networkx(graph_x)

    selected = kruskal_mst(n, edges, +weights)
    expected = [e in expected_min_x for e in graph_x]
    # println(Int.(selected .⊻ expected))
    @test selected == expected

    selected = kruskal_mst(n, edges, -weights)
    expected = [e in expected_max_x for e in graph_x]
    @test selected == expected
end


@testset "MST: Prim    Wikipedia         " begin
    n, edges, weights = from_networkx(graph_x)

    _, selected = minimum_spantree_edges(n, edges, +weights)
    expected = Set(i for (i, e) in enumerate(graph_x) if e in expected_min_x)
    @test Set(selected) == expected

    _, selected = minimum_spantree_edges(n, edges, -weights)
    expected = Set(i for (i, e) in enumerate(graph_x) if e in expected_max_x)
    @test Set(selected) == expected
end

end
