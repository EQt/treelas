module TestRotate

using Test

import TreeLas: Cycle
import GraphIdx
import GraphIdx: IncidenceIndex, Vec
import GraphIdx.Tree: ChildrenIndex, hierarchy_string
import GraphIdx.Tree: lowest_common_ancestors, root_node
import GraphIdx.Tree: dfs_finish


@testset "Rotate Grid(2, 3)              " begin
    g = GraphIdx.Grid.GridGraph(2, 3)
    if false
        GraphIdx.enumerate_edges(g) do i, u, v, _
            println(i, ":", u, " ", v)
        end
    end
    
    pi = [1, 1, 1, 3, 3, 5]
    cidx = ChildrenIndex(pi)
    @test hierarchy_string(cidx) == """
1
├─2
└─3
  ├─4
  └─5
    └─6
"""
    n = GraphIdx.num_nodes(g)
    m = GraphIdx.num_edges(g)
    lam = Vec(Float64.(collect(1:m)))
    cb = Cycle.CycleBasis(g, pi, cidx)
    @test cb.non_tree_edges == [2 => 4, 4 => 6]
    @test cb.lca == [1, 3]

    tlam0, tlam = Cycle.extract_rotate(cb, lam)
    @test tlam[2:end] ≈ [7, 10, 15, 12, 10]
    @test tlam0[2:end] ≈ [1, 4, 2, 5, 3]
end


@testset "Rotate Grid(2, 3) 2            " begin
    g = GraphIdx.Grid.GridGraph(2, 3)
    pi = [1, 1, 1, 6, 3, 5]
    cidx = ChildrenIndex(pi)
    @test hierarchy_string(cidx) == """
1
├─2
└─3
  └─5
    └─6
      └─4
"""
    n = GraphIdx.num_nodes(g)
    m = GraphIdx.num_edges(g)
    lam = Vec(Float64.(collect(1:m)))
    cb = Cycle.CycleBasis(g, pi)
    @test cb.non_tree_edges == [3 => 4, 2 => 4]
    @test cb.lca == [3, 1]

    tlam0, tlam = Cycle.extract_rotate(cb, lam)
    @test tlam0[2:end] ≈ [1, 4, 7, 5, 3]
    @test tlam[2:end] ≈ (tlam0[2:end] .+ 6) + [0, 0, 2, 2, 2]
end


@testset "Rotate Demo 3x7                " begin
    include("demo3x7.jl")
    n1, n2 = size(y)
    graph = GraphIdx.Grid.GridGraph(n1, n2)
    @test size(graph) == (n1, n2)
end


end
