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

import SparseArrays: mul!
import Printf: @sprintf
import GraphIdx.Tree: RootedTree
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx: WeightedGraph, enumerate_edges
import GraphIdx.LinA: Edges
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
    process::Function=x->nothing,
    dprocess::Function=α->nothing,
    tprocess::Function=(t,w)->nothing,
) where {E,N}
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
        dprocess(alpha)

        gap_vec!(γ, x, alpha, graph, -1.0)

        if verbose
            println(@sprintf("%4d %f", it, -sum(γ)))
        end

        prim_mst_edges(γ, root_node, mst_mem)

        tprocess(γ, parent)

        begin # non-tree edges ==> z
            z .= y
            for (i, e) in enumerate(edges)
                v, u = e
                if parent[v] == u
                    tlam[v] = lambda[i]
                elseif parent[u] == v
                    tlam[u] = lambda[i]
                else
                    z[v] -= lambda[i] * alpha[i]
                    z[u] += lambda[i] * alpha[i]
                end
            end
        end

        tree_dp!(x, z,
                 tree,
                 ArrayWeights(tlam),
                 ConstantWeights(0.5*mu),
                 dp_mem)

        process(x)

        begin # compute dual ==> update alpha
            let tree_alpha = tlam   # alpha within the tree (tlam is not needed)
                dual!(tree_alpha, x, z, dp_mem.proc_order, parent)
                @assert tree_alpha[dp_mem.proc_order[end]] ≈ 0.0
                for i in @view dp_mem.proc_order[1:end-1]
                    let eidx = selected[i], p = parent[i]
                        alpha[eidx] = tree_alpha[i] * sign(i - p)
                    end
                end
            end
        end
    end

    return x
end

end
