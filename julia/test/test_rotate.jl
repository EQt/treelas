module TestRotate

import GraphIdx
import GraphIdx: IncidenceIndex
import GraphIdx.Tree: ChildrenIndex, hierarchy_string
import GraphIdx.Tree: lowest_common_ancestors, root_node
import GraphIdx.Tree: dfs_finish
using Test


struct CycleBasis
    non_tree_edges::Vector{Pair{Int,Int}}
    non_tree_enum::Vector{Int}
    tree_enum::Vector{Int}
    non_tree_idx::IncidenceIndex
    lca::Vector{Int}
end


GraphIdx.num_nodes(cb::CycleBasis) =
    length(cb.tree_enum)


"""
    extract_cyclebasis(graph, pi, cidx)

Construct a cycle basis given a tree with parents `pi`
and `cidx::ChildrenIndex`.
"""
function extract_cyclebasis(
    g::Graph,
    pi::Vector{Int},
    cidx::ChildrenIndex,
)::CycleBasis where {Graph}
    n = GraphIdx.num_nodes(g)
    m = GraphIdx.num_edges(g)
    non_tree = Vector{Pair{Int,Int}}()
    non_tree_enum = Vector{Int}()
    sizehint!(non_tree, m - n + 1)
    sizehint!(non_tree_enum, m - n + 1)
    tree_enum = Vector{Int}(undef, n)
    tree_enum[root_node(cidx)] = 0
    GraphIdx.enumerate_edges(g) do i, u, v, _
        if pi[u] == v
            tree_enum[u] = i
        elseif pi[v] == u
            tree_enum[v] = i
        else
            push!(non_tree, u => v)
            push!(non_tree_enum, i)
        end
    end
    nidx = IncidenceIndex(n, non_tree)
    lca = lowest_common_ancestors(cidx, pi, nidx)
    CycleBasis(
        non_tree,
        non_tree_enum,
        tree_enum,
        nidx,
        lca,
    )
end    


"""
    enumerate_cycles(func, cb)

For every cycle Call function `func(ei, u, v, r)` whereby
- `ei` is the index of the corresponding edge `(u, v)`
- `r` is the root of the cycle, i.e. the lowest common ancestor of `u` and `v`

Notice that for every edge `(u, v)` the function is called twice:
With `(u, v)` and `(v, u)`.
"""
function enumerate_cycles(func::F, cb::CycleBasis) where {F<:Function}
    n = GraphIdx.num_nodes(cb)
    for v = 1:n
        for (u, i) in cb.non_tree_idx[v]
            r = cb.lca[i]
            func(i, u, v, r)
        end
    end
end



"""
    extract_rotate(graph, lam, pi, cidx)


"""
function extract_rotate(
    graph::Graph,
    lam::Wlam,
    pi::Vector{Int},
    cidx::ChildrenIndex,
) where {Graph,Wlam}
    n = GraphIdx.num_nodes(graph)
    m = GraphIdx.num_edges(graph)
    cb = extract_cyclebasis(graph, pi, cidx)
    tlam = Vector{Float64}(undef, n)
    for (i, ei) in enumerate(cb.tree_enum)
        tlam[i] = ei > 0 ? lam[ei] : NaN
    end
    ldiff = zeros(Float64, n)
    enumerate_cycles(cb) do i, _, v, r
        let lami = lam[cb.non_tree_enum[i]]
            ldiff[v] += lami
            ldiff[r] -= lami
        end
    end
    tlam0 = copy(tlam)
    for v in dfs_finish(pi)
        tlam[v] += ldiff[v]
        ldiff[pi[v]] += ldiff[v]
        ldiff[v] = 0
    end
    return tlam0, tlam, cb.non_tree_edges, cb.lca
end


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
    lam = Float64.(collect(1:m))

    tlam0, tlam, non_tree, lca = extract_rotate(g, lam, pi, cidx)
    @test tlam[2:end] ≈ [7, 10, 15, 12, 10]
    @test tlam0[2:end] ≈ [1, 4, 2, 5, 3]
    @test non_tree == [2 => 4, 4 => 6]
    @test lca == [1, 3]
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
    lam = Float64.(collect(1:m))

    tlam0, tlam, non_tree, lca = extract_rotate(g, lam, pi, cidx)
    @test non_tree == [3 => 4, 2 => 4]
    @test lca == [3, 1]
    @test tlam0[2:end] ≈ [1, 4, 7, 5, 3]
    @test tlam[2:end] ≈ (tlam0[2:end] .+ 6) + [0, 0, 2, 2, 2]
end

end
