"""
Maximum Gap Tree
================

Also called **Gap Tree Lasso (GapLas)**.

In the central function `gaplas`, a tree having the largest gap
values (computed by `gap_vec!`) is selected.  The non-tree edge-flows
are forwarded into the input `y`.  Then the tree solver is used and
the tree-edges are updated.

Graph Indexes
-------------

For the different steps, several indexes are necessary
1. [`Dual.gap_vec!`](@ref) needs to access every edge once.

2. Prim's minimum spanning tree: `NeighborIndex` providing outgoing
   edges for a node (i.e. every edge is included in two node's
   adjacency lists)

3. Once the tree has been determined we need to have a `ChildrenIndex`
   from the `parent` (e.g. for `dfs_walk`); also needed for computing
   the `Queues` layout (parenthesis tree representation) and the
   processing order.


Refactoring
-----------

In principle, it is not necessary to actually compute and store the
edges (and weights); it suffices to have access via `enumerate_edges`
or alike.

"""
module MGT

import Statistics
import Printf: @sprintf
import GraphIdx.Tree: RootedTree
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx: WeightedGraph, enumerate_edges
import GraphIdx: ConstantWeights, ArrayWeights
import ..TreeDP: TreeDPMem, tree_dp!
import ..Utils: sum2, primal_objective
import ..Dual: dual!, gap_vec!, primal_from_dual!


function extract_non_tree!(z, tlam, edges, parent, alpha, lambda)
    for (i, (u, v)) in enumerate(edges)
        if parent[v] == u
            tlam[v] = lambda[i]
        elseif parent[u] == v
            tlam[u] = lambda[i]
        else
            z[u] += alpha[i]
            z[v] -= alpha[i]
        end
    end
end


"""
    gaplas(y, edges, λ)

Optimize in each iteration along a tree.
"""
function gaplas(
    y::Array{Float64,N},
    edges::Vector{E},
    lambda::Vector{Float64};
    root_node::Int = 1,
    mu::Float64 = 1.0,
    max_iter::Integer = 3,
    verbose::Bool = true,
    process::Fu1 = x->nothing,
    dprocess::Fu2 = α->nothing,
    tprocess::Fu3 = (t,w)->nothing,
)::Array{Float64,N} where {E,N,Fu1<:Function,Fu2<:Function,Fu3<:Function}
    local m = length(edges)
    local n = length(y)
    local alpha = zeros(m)
    local γ = zeros(m)
    local x = copy(y)
    local z = similar(y)
    local tlam = Vector{Float64}(undef, n)
    local dp_mem = TreeDPMem(n)
    local mst_mem = PrimMstMem(edges, n)
    local selected = mst_mem.selected
    local parent = mst_mem.parent
    local tree = RootedTree(root_node, parent)
    local graph::WeightedGraph = WeightedGraph(mst_mem.neighbors, lambda)
    if verbose
        local γ_sorted = similar(γ)
        local x2 = similar(x)
    end

    for it in 1:max_iter
        gap_vec!(γ, x, alpha, graph, -1.0)
        if verbose
            if n < 30 && it > 1
                tree_gamma_check(γ, alpha, tlam, selected,
                                 x, z, dp_mem.proc_order, parent)
            end
            local quant = Statistics.quantile!(γ_sorted .= γ .* -1,
                                               [0.90, 0.95, 0.98])
            local gap = -sum(γ)
            local dual_obj = 0.5*sum2(x)
            local prim_obj = primal_objective(x, y, graph)
            primal_from_dual!(x2 .= y, alpha, graph)
            @assert x ≈ x2
            @assert prim_obj + dual_obj - gap ≈ 0.5sum2(y)

            println(@sprintf("%4d %12.4f %12.4f %12.4f",
                              # "%8f %8f %8f",
                             it,
                             gap,
                             dual_obj,
                             prim_obj,
                             ))
                             # quant...))
        end

        prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)
        z .= y
        extract_non_tree!(z, tlam, edges, parent, alpha, lambda)
        tree_dp!(x, z, tree, ArrayWeights(tlam), ConstantWeights(mu), dp_mem)
        process(x)
        let tree_alpha = tlam   # alpha within the tree (tlam is not needed)
            tree_alpha .= vec(x) .- vec(z)
            dual!(tree_alpha, dp_mem.proc_order, parent)
            update_tree!(alpha, tree_alpha, selected, edges, parent)
            duality_check(alpha, lambda)
        end
        dprocess(alpha)

    end

    return x
end


"""
    gaplas(...)

Graph has to implement several methods:
- `iter_edges(::Function, ::Graph)`
- `NeighborIndex(::Graph)`
"""
function gaplas(y::Array{Float64}, g::Graph) where {Graph}
    x = copy(y)
    α = zeros(num_edges(g))
    error("notimplemented")
end


"""
    gaplas!(...)

Perform one iteration. Premises:
1. `x = y + D'*α`
2. `α` dually feasible, `abs(α[e]) ≤ λ[e]` for all edges `e`.
"""
function gaplas!(x::Array{Float64}, α::Vector{Float64}, γ::Vector{Float64},
                 y::Array{Float64}, g::Graph) where {Graph}
    error("notimplemented")
end


function update_tree!(alpha, tree_alpha, selected, edges, parent)
    @assert selected[1] < 0
    for i in @view selected[2:end]
        local u::Int, v::Int = edges[i]
        alpha[i] = if parent[v] == u
            -tree_alpha[v]
        elseif parent[u] == v
            +tree_alpha[u]
        else
            error("Should not happen")
        end
    end
end


function duality_check(alpha, lambda)
    for i in 1:length(alpha)
        @assert(abs(alpha[i]) <= (1 + 1e-8)*lambda[i],
                "$i: $u--$v " *
                "$(alpha[i]) < $(lambda[i])?")
    end
end


function tree_gamma_check(γ, alpha, tlam, selected, x, z, proc_order, parent)
    let γ = round.(-γ, digits=2)
        # @show γ
    end

    @assert selected[1] < 0
    for i in @view selected[2:end]
        if abs(γ[i]) >= 1e-15
            talp = vec(x) - vec(z)
            for i in @view proc_order[1:end-1]
                talp[parent[i]] += talp[i]
            end
            alp = zeros(m)
            for i in @view selected[2:end]
                u, v = edges[i]
                alp[i] = parent[u] == v ? talp[u] : -talp[v]
            end
            for i in 1:m
                println(@sprintf("%7.3f  %7.3f", alp[i], alpha[i]))
            end
            @assert(
                abs(γ[i]) < 1e-15,
                "i=$i: $(edges[i])\n γ=$(γ[i])\n α=$(alpha[i])" *
                "\n $(x[edges[i][1]]) $(x[edges[i][2]])" *
                "\n $(dp_mem.proc_order)" *
                "\n $(y≈z)" *
                "\n $(talp ≈ tlam)" * ""
            )
        end
    end
end


end
