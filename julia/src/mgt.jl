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

2. Prim's minimum spanning tree: `IncidenceIndex` providing outgoing
   edges for a node (i.e. every edge is included in two node's
   adjacency lists)

3. Once the tree has been determined we need to have a `ChildrenIndex`
   from the `parent` (e.g. for `dfs_walk`); also needed for computing
   the `Queues` layout (parenthesis tree representation) and the
   processing order.
"""
module MGT

import Statistics
import Printf: @sprintf
import GraphIdx.Tree: RootedTree
import GraphIdx: PrimMstMem, prim_mst_edges, Graph, EdgeGraph, Edge
import GraphIdx: WeightedGraph, enumerate_edges, num_edges, num_nodes
import GraphIdx: Const, Vec, Ones, Weights
import ..TreeDP: TreeDPMem, tree_dp!
import ..Utils: sum2, primal_objective
import ..Dual: dual!, gap_vec!, primal_from_dual!


"""
    extract_non_tree!(graph, y, λt, π, α, λ)

Iterate over all edges of `grapy` (weighted by `λ`) and do the following:
If the edge `e` is a tree edge, store the corresponding `λ[e]` in tree order.
If the edge `e` is not part of the tree, compute the flow `α[e]` into the
node input `y`.
"""
function extract_non_tree!(
    graph::Graph,
    parent::Vector{Int},
    y::Array,
    alpha::Array,
    tlam::W,
    lambda::W,
) where {W}
    enumerate_typed_edges(graph, parent) do istree::Bool, i::Int, u::Int, v::Int
        if istree
            tlam[u] = lambda[i]
        else
            y[u] += alpha[i]
            y[v] -= alpha[i]
        end
    end
end


"""
    traverse_edges(func, edges, π)

Call for every edge `(u, v)` enumerated by `i` the function
```julia
func(istree, i, u, v)
```

whereby `istree` tells whether the edge is a tree edge.
If it is a tree edge, `u` is the child of `v`.
"""
function enumerate_typed_edges(func::Function, graph::Graph, parent::Vector{Int})
    enumerate_edges(graph) do i::Int, u::Int, v::Int
        if parent[v] == u
            func(true, i, v, u)
        elseif parent[u] == v
            func(true, i, u, v)
        else
            func(false, i, u, v)
        end
    end
end


"""
    gaplas(y, graph, λ; learn=1.0, max_iter=3, ...)

Optimize in each iteration along a tree.

The `learn` parameter controls how much of the new tree
solution should be taken (should be between ``0`` and ``1.0``).
"""
function gaplas(
    y::Array{Float64,N},
    graph::EdgeGraph,
    lambda::Vector{Float64};
    root_node::Int = 1,
    mu::Wmu = Ones{Float64}(),
    max_iter::Integer = 3,
    verbose::Bool = true,
    process::Fu1 = x->nothing,
    dprocess::Fu2 = α->nothing,
    tprocess::Fu3 = (t,w)->nothing,
    learn::Float64 = 1.0,
)::Array{Float64,N} where {N, Fu1<:Function, Fu2<:Function, Fu3<:Function, Wmu}
    local m = num_edges(graph)
    local n = length(y)
    @assert n == num_nodes(graph)
    local alpha = zeros(m)
    local γ = zeros(m)
    local x = copy(y)
    local z = similar(y)
    local tlam = Vector{Float64}(undef, n)
    local dp_mem = TreeDPMem(n)
    local mst_mem = PrimMstMem(graph)
    local selected = mst_mem.selected
    local parent = mst_mem.parent
    local tree = RootedTree(root_node, parent)
    local wgraph::WeightedGraph = WeightedGraph(mst_mem.neighbors, lambda)
    local x_new::Array{Float64,N} = learn >= 1.0 ? x : copy(x)
    local alpha_new::Array{Float64} = learn >= 1.0 ? alpha : copy(alpha)
    if verbose
        local γ_sorted = similar(γ)
        local x2 = similar(x)
    end

    for it in 1:max_iter
        gap_vec!(γ, x, alpha, wgraph, -1.0)
        if verbose
            if n < 30 && it > 1 && false
                tree_gamma_check(γ, alpha, tlam, selected,
                                 x, z, dp_mem.proc_order, parent)
            end
            local quant = Statistics.quantile!(γ_sorted .= γ .* -1,
                                               [0.90, 0.95, 0.98])
            local gap = -sum(γ)
            local dual_obj = 0.5*sum2(x)
            local prim_obj = primal_objective(x, y, wgraph)
            primal_from_dual!(x2 .= y, alpha, wgraph)
            # @assert x ≈ x2 "‖x - x2‖_∞ = $(maximum(abs.(x - x2)))"
            @assert prim_obj + dual_obj - gap ≈ 0.5sum2(y)

            print(@sprintf("%4d %12.4f %12.4f %12.4f",
                           it,
                           gap,
                           dual_obj,
                           prim_obj,
                           ))

            if false
                print(@sprintf("%8f %8f %8f", quant...))
            end
            println()
        end

        prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)
        z .= y
        extract_non_tree!(graph, parent, z, alpha, tlam, lambda)
        x_new .= x
        tree_dp!(x_new, z, tree, Vec(tlam), mu, dp_mem)
        if !(x === x_new)
            x .= (1 - learn) .* x .+ learn .* x_new
        end
        process(x)
        let tree_alpha = tlam   # alpha within the tree (tlam is not needed)
            tree_alpha .= vec(x_new) .- vec(z)
            dual!(tree_alpha, dp_mem.proc_order, parent)
            alpha_new .= alpha
            update_tree!(alpha_new, tree_alpha, selected, graph, parent)
            if !(alpha === alpha_new)
                alpha .= (1 - learn) .* alpha .+ learn .* alpha_new
            end
            duality_check(alpha, lambda)
        end
        dprocess(alpha)
    end

    return x
end

gaplas(y::Array, edges, λ::Float64; args...) =
    gaplas(y, edges, fill(λ, length(edges)); args...)

gaplas(y::Array, edges::Vector{Edge{Int}}, λ::Vector{Float64}; args...) where {E} =
    gaplas(y, EdgeGraph(length(y), edges), λ; args...)


"""
    gaplas(...)

Graph has to implement several methods:
- `iter_edges(::Function, ::Graph)`
- `IncidenceIndex(::Graph)`
"""
function gaplas(
    y::Array{F,N},
    g::Graph,
    λ::Weights,
    μ::Weights = Ones{F}(),
)::Array{F,N} where {F, N}
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
                 y::Array{Float64}, g::Graph)
    error("notimplemented")
end


"""
    update_tree!(α, αt, selected, graph, parent)

Update the global dual `α` by a tree dual `αt`.
"""
function update_tree!(alpha, tree_alpha, selected, graph::EdgeGraph, parent)
    @assert selected[1] < 0
    for i in @view selected[2:end]
        local u::Int, v::Int = graph.edges[i]
        alpha[i] = if parent[v] == u
            -tree_alpha[v]
        elseif parent[u] == v
            +tree_alpha[u]
        else
            error("Should not happen")
        end
    end
end


"""
Ensure that the alpha is dually feasible, i.e.
componentwise absolutely not greater than lambda.
"""
function duality_check(alpha, lambda)
    for i in 1:length(alpha)
        @assert(abs(alpha[i]) <= (1 + 1e-8)*lambda[i],
                "$i: $u--$v " *
                "$(alpha[i]) < $(lambda[i])?")
    end
end


function tree_gamma_check(γ, alpha, tlam, selected, x, z, proc_order, parent, edges)
    let γ = round.(-γ, digits=2)
        # @show γ
    end
    m = length(alpha)

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
