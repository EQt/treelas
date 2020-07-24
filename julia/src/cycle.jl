"""
Given a graph `g` and a rooted tree `t` within `g`, we can decompose `g` into cycles:

Every non-tree edge `(u,v)` of `g` defines a cycle, i.e. the
unique path `v --> u` within the tree `t` plus the edge `(u, v)` to close it.

This module contains functions to compute within the cycle space.
"""
module Cycle

import GraphIdx
import GraphIdx: IncidenceIndex, Weights, Graph
import GraphIdx.Tree: ChildrenIndex, root_node, lowest_common_ancestors, dfs_finish


"""
All kind of indices to represant a cycle basis corresponding to spanning tree
"""
mutable struct CycleBasis
    pi::Vector{Int}
    cidx::ChildrenIndex
    non_tree_edges::Vector{Pair{Int,Int}}
    non_tree_enum::Vector{Int}
    tree_enum::Vector{Int}
    non_tree_idx::IncidenceIndex
    lca::Vector{Int}
end


GraphIdx.num_nodes(cb::CycleBasis) =
    length(cb.tree_enum)

GraphIdx.num_edges(cb::CycleBasis) =
    GraphIdx.num_nodes(cb) + length(cb.non_tree_enum)


"""
    CycleBasis(graph)
    CyccleBasis(m, n)

If no tree is provided, just allocate the space.
"""
CycleBasis(g::Graph)::CycleBasis =
    CycleBasis(GraphIdx.num_edges(g), GraphIdx.num_nodes(g))

function CycleBasis(m::Integer, n::Integer)
    non_tree_edges = Vector{Pair{Int,Int}}()
    non_tree_enum = Vector{Int}()
    sizehint!(non_tree_edges, m - n + 1)
    sizehint!(non_tree_enum, m - n + 1)
    tree_enum = Vector{Int}(undef, n)
    return CycleBasis(
        [],                     # pi
        ChildrenIndex([], []),  # cidx
        non_tree_edges,
        non_tree_enum,
        tree_enum,
        IncidenceIndex([], []), # non_tree_idx
        [],                     # lca
    )
end

"""
    CycleBasis(graph, pi, [cidx])

Construct a cycle basis given a tree with parents `pi` and `cidx::ChildrenIndex`.
"""
function CycleBasis(g::Graph, pi::Vector{Int}, cidx::ChildrenIndex)::CycleBasis
    n = GraphIdx.num_nodes(g)
    m = GraphIdx.num_edges(g)
    non_tree = Vector{Pair{Int,Int}}()
    non_tree_enum = Vector{Int}()
    sizehint!(non_tree, m - n + 1)
    sizehint!(non_tree_enum, m - n + 1)
    tree_enum = Vector{Int}(undef, n)
    tree_enum[root_node(cidx)] = 0
    GraphIdx.enumerate_edges(g) do i::Int, u::Int, v::Int
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
        pi,
        cidx,
        non_tree,
        non_tree_enum,
        tree_enum,
        nidx,
        lca,
    )
end    

CycleBasis(g::Graph, pi::Vector{Int}) =
    CycleBasis(g, pi, ChildrenIndex(pi))


"""
    enumerate_non_tree(func, cb)

For every cycle call function `func(ei, u, v, r)` whereby
- `ei` is the index of the corresponding non-tree edge `(u, v)`
- `r` is the root of the cycle, i.e. the lowest common
  ancestor of `u` and `v` within the tree

Notice that for every edge `(u, v)` the function is called twice:
With `(u, v)` and `(v, u)`.
"""
@inline function enumerate_non_tree(func::F, cb::CycleBasis) where {F<:Function}
    n = GraphIdx.num_nodes(cb)
    for v = 1:n
        for (u, i) in cb.non_tree_idx[v]
            r = cb.lca[i]
            func(i, u, v, r)
        end
    end
end


"""
    extract_rotate(cb, λ)

Return the weights
TODO: finish
"""
function extract_rotate(
    cb::CycleBasis, lam::Weights{F}
)::Tuple{Vector{F}, Vector{F}} where {F}
    n = GraphIdx.num_nodes(cb)
    tlam = Vector{Float64}(undef, n)
    tlam0 = Vector{Float64}(undef, n)
    ldiff = zeros(Float64, n)

    for (i, ei) in enumerate(cb.tree_enum)
        tlam[i] = ei > 0 ? lam[ei] : NaN
    end
    enumerate_non_tree(cb) do i, _, v, r
        let lami = lam[cb.non_tree_enum[i]]
            ldiff[v] += lami
            ldiff[r] -= lami
        end
    end
    tlam0 .= tlam
    for v in dfs_finish(cb.pi)
        tlam[v] += ldiff[v]
        ldiff[cb.pi[v]] += ldiff[v]
        ldiff[v] = 0
    end
    return tlam0, tlam
end


end
