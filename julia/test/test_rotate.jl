module TestRotate

import GraphIdx
import GraphIdx: IncidenceIndex
import GraphIdx.Tree: ChildrenIndex, hierarchy_string, lowest_common_ancestors
import GraphIdx.Tree: dfs_finish
using Test

@testset "Rotate Grid(2, 3)               " begin
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
    lam = Float64.(collect(1:m))

    non_tree = Vector{Pair{Int,Int}}()
    non_tree_enum = Vector{Int}()
    sizehint!(non_tree, m - n + 1)
    sizehint!(non_tree_enum, m - n + 1)
    tlam = fill(NaN, n)
    GraphIdx.enumerate_edges(g) do i, u, v, _
        if pi[u] == v
            tlam[u] = lam[i]
        elseif pi[v] == u
            tlam[v] = lam[i]
        else
            push!(non_tree, u => v)
            push!(non_tree_enum, i)
        end
    end
    @test non_tree == [2 => 4, 4 => 6]
    nidx = IncidenceIndex(n, non_tree)
    lca = lowest_common_ancestors(cidx, pi, nidx)
    @test lca == [1, 3]
    ldiff = zeros(Float64, n)
    for v = 1:n
        for (_, i) in nidx[v]
            let lami = lam[non_tree_enum[i]]
                ldiff[v] += lami
                ldiff[lca[i]] -= lami
            end
        end
    end

    for v in dfs_finish(pi)
        tlam[v] += ldiff[v]
        ldiff[pi[v]] += ldiff[v]
        ldiff[v] = 0
    end
    @test tlam[2:end] ≈ [7, 10, 15, 12, 10]
end

end
