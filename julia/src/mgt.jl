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
            println(@sprintf("%4d %f", it, -sum(γ)))
        end

        # @show γ
        prim_mst_edges(γ, root_node, mst_mem)
        tprocess(γ, parent)
        # @show tree
        # @show selected

        begin # non-tree edges ==> z
            z .= y
            for (i, (u, v)) in enumerate(edges)
                if parent[v] == u
                    @show i
                    tlam[v] = lambda[i]
                elseif parent[u] == v
                    @show i
                    tlam[u] = lambda[i]
                else
                    let alpha_i = round.(alpha[i], digits=1)
                        # @show i, alpha_i
                    end
                    z[v] -= alpha[i]
                    z[u] += alpha[i]
                end
            end
        end

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
            # @show tree_alpha
            @show "alpha update"
            for i in @view selected[2:end]
                @show i
                local u::Int, v::Int = edges[i]
                @assert u < v
                alpha[i] = if parent[v] == u
                    # @show (i, u, v)
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
