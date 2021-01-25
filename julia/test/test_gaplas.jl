module TestGapLas

using Test
import TreeLas: GapLas
import GraphIdx

@testset "GapLas  : demo 3x7             " begin
    include("demo3x7.jl")
    graph = GraphIdx.Grid.GridGraph(size(y)...)
    λ = GraphIdx.Ones{Float64}()
    μ = GraphIdx.Ones{Float64}()
    mem = GapLas.GapMem(y, graph, λ)
    GapLas.gaplas!(mem, y, graph, λ, μ)
    @testset "tree edges" begin
        parent = copy(mem.tree.parent)
        tree_edges = Dict{NTuple{2,Int},Int}()
        GraphIdx.enumerate_edges(graph) do e::Int, u::Int, v::Int
            if parent[v] == u || parent[u] == v
                tree_edges[(u, v)] = e
            end
        end
        tree_sel = Dict(mem.egraph.edges[s] => s for s in @view mem.mst.selected[2:end])
        @test tree_sel == tree_edges
        # be able to compare to c++ implementation
        @test join(
            (
                "{{$(e[1] -1), $(e[2] -1)}, $a}"
                for (e, a) in zip(mem.egraph.edges, round.(mem.sol.α, digits=3))
            ),
            ",\n"
        ) == """
        {{0, 1}, 0.0},
        {{3, 4}, 0.248},
        {{6, 7}, -0.273},
        {{9, 10}, 0.0},
        {{12, 13}, 0.0},
        {{15, 16}, -0.171},
        {{18, 19}, -0.238},
        {{1, 2}, 0.0},
        {{4, 5}, -0.505},
        {{7, 8}, -1.0},
        {{10, 11}, 0.0},
        {{13, 14}, -0.758},
        {{16, 17}, -0.266},
        {{19, 20}, 0.0},
        {{0, 3}, 0.179},
        {{3, 6}, 0.0},
        {{6, 9}, 0.405},
        {{9, 12}, -0.254},
        {{12, 15}, -0.582},
        {{15, 18}, 0.0},
        {{1, 4}, 0.179},
        {{4, 7}, 0.0},
        {{7, 10}, 0.618},
        {{10, 13}, 0.0},
        {{13, 16}, 0.0},
        {{16, 19}, -0.223},
        {{2, 5}, -0.101},
        {{5, 8}, -0.127},
        {{8, 11}, 0.151},
        {{11, 14}, 0.0},
        {{14, 17}, -0.997},
        {{17, 20}, -0.442}
        """ |> chomp
    end
    @test mem.dp_mem.proc_order == [
        12, 11, 19, 20, 21, 14, 15, 18, 17, 16, 13, 10, 7, 8, 9, 3, 6, 2, 5, 4, 1
    ]
    @test round.(mem.α_tree, digits=3) ≈ [
        -0.0, 0.179, -0.101, -0.179, -0.248, 0.505, -0.273,
        -1.0, 0.127, -0.405, -0.618, -0.151, 0.254, -0.758,
        -0.997, 0.582, 0.171, 0.266, -0.238, 0.223, 0.442
    ]
    @test round.(mem.sol.α, digits=3) ≈ [
        +0.0,   +0.248, -0.273, +0.0,   +0.0,   -0.171, -0.238, +0.0,
        -0.505, -1.0,   +0.0,   -0.758, -0.266, +0.0,   +0.179, +0.0,
        +0.405, -0.254, -0.582, +0.0,   +0.179, +0.0,   +0.618, +0.0,
        +0.0,   -0.223, -0.101, -0.127, +0.151, +0.0,   -0.997, -0.442
    ]
end


@testset "GapLas  : demo 3x7 iter 5      " begin
    include("demo3x7.jl")
    graph = GraphIdx.Grid.GridGraph(size(y)...)
    lam = GraphIdx.Const(0.1);
    GapLas.gaplas(y, graph, lam; max_iter=5, verbose=false)
end


end
