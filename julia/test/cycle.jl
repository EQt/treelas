module Cycle

import GraphIdx
import GraphIdx: IncidenceIndex, Weights, Graph
import GraphIdx.Tree: ChildrenIndex, root_node, lowest_common_ancestors, dfs_finish


"""
All kind of indices to represant a cycle basis corresponding to spanning tree
"""
struct CycleBasis
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
    extract_cyclebasis(graph, pi, cidx)

Construct a cycle basis given a tree with parents `pi`
and `cidx::ChildrenIndex`.
"""
function extract_cyclebasis(
    g::Graph,
    cidx::ChildrenIndex,
    pi::Vector{Int},
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
        pi,
        cidx,
        non_tree,
        non_tree_enum,
        tree_enum,
        nidx,
        lca,
    )
end    


"""
    enumerate_non_tree(func, cb)

For every cycle call function `func(ei, u, v, r)` whereby
- `ei` is the index of the corresponding non-tree edge `(u, v)`
- `r` is the root of the cycle, i.e. the lowest common
  ancestor of `u` and `v` within the tree

Notice that for every edge `(u, v)` the function is called twice:
With `(u, v)` and `(v, u)`.
"""
function enumerate_non_tree(func::Function, cb::CycleBasis)
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
"""
function extract_rotate(cb::CycleBasis, lam::Weights)
    n = GraphIdx.num_nodes(cb)
    m = GraphIdx.num_edges(cb)
    tlam = Vector{Float64}(undef, n)
    for (i, ei) in enumerate(cb.tree_enum)
        tlam[i] = ei > 0 ? lam[ei] : NaN
    end
    ldiff = zeros(Float64, n)
    enumerate_non_tree(cb) do i, _, v, r
        let lami = lam[cb.non_tree_enum[i]]
            ldiff[v] += lami
            ldiff[r] -= lami
        end
    end
    tlam0 = copy(tlam)
    for v in dfs_finish(cb.pi)
        tlam[v] += ldiff[v]
        ldiff[cb.pi[v]] += ldiff[v]
        ldiff[v] = 0
    end
    return tlam0, tlam
end


@deprecate(
    extract_rotate(graph::Graph, cidx::ChildrenIndex, pi::Vector{Int}, lam::Weights),
    let cb = extract_cyclebasis(graph, cidx, pi)
        extract_rotate(cb, lam)..., cb.non_tree_edges, cb.lca
    end
)

end
