"""
# Maximum Gap Tree

In the central function `gaplas`, a tree having the largest gap
values (computed by `gap_vec!`) is selected.  The non-tree edge-flows
are forwarded into the input `y`.  Then the tree solver is used and
the tree-edges are updated.

## Refactoring

Prim's MST algorithm needs an index telling for each node the
neighbors.  The same is needed for DFS traversal.  Furtheron, an edge
order need to be specified, i.e. we should provide an edge iterator.

"""
module MGT

include("gap.jl")
include("dual.jl")

import Printf: @sprintf
import GraphIdx.Tree: RootedTree
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx: WeightedGraph, enumerate_edges
import ..TreeDP: TreeDPMem, tree_dp!, ConstantWeights, ArrayWeights


@inline function extract_non_tree!(z, tlam, edges, parent, alpha, lambda)
    for (i, (u, v)) in enumerate(edges)
        if parent[v] == u
            tlam[v] = lambda[i]
        elseif parent[u] == v
            tlam[u] = lambda[i]
        else
            z[v] -= alpha[i]
            z[u] += alpha[i]
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
    max_iter::Int = 3,
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

    for it in 1:max_iter
        gap_vec!(γ, x, alpha, graph, -1.0)
        if verbose
            if n < 30
                let γ = round.(-γ, digits=2)
                    @show γ
                end
                if it > 1
                    for i in selected
                        if i >= 1
                            @assert(abs(γ[i]) < 1e-15,
                                    "i=$i: $(edges[i])\n γ=$(γ[i])\n α=$(alpha[i])" *
                                    "\n $(x[edges[i][1]]) $(x[edges[i][2]])")
                        end
                    end
                end
            end
            local obj::Ref{Float64} = Ref{Float64}(
                0.5 * sum((x[i] - y[i])^2 for i = 1:n))
            enumerate_edges(graph) do ei::Int, u::Int, v::Int, lam::Float64
                obj[] += lam * abs(x[u] - x[v])
            end
            println(@sprintf("%4d %7.4f %7.4f", it, -sum(γ), obj[]))
        end

        prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)

        z .= y
        extract_non_tree!(z, tlam, edges, parent, alpha, lambda)

        tree_dp!(x,
                 z,
                 tree,
                 ArrayWeights(tlam),
                 ConstantWeights(mu),
                 dp_mem)
        process(x)

        # compute dual ==> update alpha
        let tree_alpha = tlam   # alpha within the tree (tlam is not needed)
            dual!(tree_alpha, x, z, dp_mem.proc_order, parent)
            for i in @view selected[2:end]
                local u::Int, v::Int = edges[i]
                alpha[i] = if parent[v] == u
                    +tree_alpha[u]
                elseif parent[u] == v
                    -tree_alpha[v]
                else
                    error("Should not happen")
                end
                @assert(abs(alpha[i]) <= (1 + 1e-8)*lambda[i],
                        "$i: $u--$v " *
                        "$(alpha[i]) < $(lambda[i])?")
            end
        end
        dprocess(alpha)

    end

    return x
end

end
