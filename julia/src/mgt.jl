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

import GraphIdx
import GraphIdx: Const, Vec, Ones, Weights
import GraphIdx: PrimMstMem, prim_mst_edges, Graph, EdgeGraph, Edge
import GraphIdx: WeightedGraph, enumerate_edges, num_edges, num_nodes
import GraphIdx.Tree: RootedTree

import ..TreeDP: TreeDPMem, tree_dp!
import ..Utils: sum2, primal_objective
import ..Dual: dual!, gap_vec!, primal_from_dual!
import ..TreeFlow: update_tree!, extract_non_tree!


"""
    gaplas(y, graph, λ; learn=1.0, max_iter=3, ...)

Optimize in each iteration along a tree.

The `learn` parameter controls how much of the new tree
solution should be taken (should be between ``0`` and ``1.0``).
"""
function gaplas(
    y::Array{Float64,N},
    graph::EdgeGraph,
    lambda::Weights{Float64};
    root_node::Int = 1,
    mu::Wmu = Ones{Float64}(),
    max_iter::I = 3,
    verbose::Bool = true,
    process::Fu1 = x->nothing,
    dprocess::Fu2 = α->nothing,
    tprocess::Fu3 = (t,w)->nothing,
    learn::Float64 = 1.0,
)::Array{Float64,N} where {N, I<:Integer, Fu1<:Function, Fu2<:Function, Fu3<:Function, Wmu}
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
    local wgraph::WeightedGraph = WeightedGraph(mst_mem.neighbors, collect(lambda, m))
    local x_new::Array{Float64,N} = learn >= 1.0 ? x : copy(x)
    local alpha_new::Array{Float64} = learn >= 1.0 ? alpha : copy(alpha)
    if verbose
        local γ_sorted = similar(γ)
        local x2 = similar(x)
    end

    for it in 1:max_iter
        @timed begin
        gap_vec!(γ, x, alpha, wgraph, -1.0)
        @timed if verbose
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

        @timed prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)
        z .= y
        extract_non_tree!(graph, parent, z, alpha, tlam, lambda)
        x_new .= x
        @timed tree_dp!(x_new, z, tree, Vec(tlam), mu, dp_mem)
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
            if false
                duality_check(alpha, lambda)
            end
        end
        dprocess(alpha)
        end
    end

    return x
end


@deprecate(
    gaplas(y::Array, edges, λ::Array{Float64}; args...),
    gaplas(y, edges, GraphIdx.Vec(λ); args...),
)


@deprecate(
    gaplas(y::Array, edges, λ::Float64; args...),
    gaplas(y, edges, GraphIdx.Const(λ); args...),
)


gaplas(y::Array, edges::Vector{Edge{Int}}, λ::Weights{Float64}; args...) =
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
    μ::Weights = Ones{F}();
    args...
)::Array{F,N} where {F, N}
    edge_graph::EdgeGraph = collect(g)
    gaplas(y, edge_graph, λ; mu=μ, args...)
end


"""
    gaplas!(...)

Perform one iteration. Premises:
1. `x = y + D'*α`
2. `α` dually feasible, `abs(α[e]) ≤ λ[e]` for all edges `e`.
"""
function gaplas!(
    x::Array{Float64}, α::Vector{Float64}, γ::Vector{Float64}, y::Array{Float64}, g::Graph
)
    error("notimplemented")
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
