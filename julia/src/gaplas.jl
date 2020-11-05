"""
Re-factoring and re-implementation of MGT
"""
module GapLas

import Printf: @printf
import GraphIdx
import GraphIdx: Graph, EdgeGraph, Edge, Weights, WeightedGraph
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx.Tree: RootedTree

import ..Dual: dual!, gap_vec!
import ..TreeDP: TreeDPMem, tree_dp!
import ..TreeFlow: extract_non_tree!, update_tree!


struct GapMem{N, WL<:Weights{Float64}}
    x::Array{Float64,N}
    z::Array{Float64,N}
    alpha::Vector{Float64}
    gamma::Vector{Float64}
    tree_lam::WL
    dp_mem::TreeDPMem
    mst::PrimMstMem
    wgraph::WeightedGraph
    tree::RootedTree
    tree_alpha::Vector{Float64}
    egraph::EdgeGraph
end


function GapMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    root_node = 1
    m = GraphIdx.num_edges(graph)
    n = GraphIdx.num_nodes(graph)
    @assert length(y) == n
    tree_lam = similar(lambda, n)
    tree_alpha = Vector{Float64}(undef, n)
    egraph = collect(graph)
    mst = PrimMstMem(egraph)
    lam = Float64[lambda[i] for i=1:m]
    tree = RootedTree(root_node, mst.parent)
    return GapMem(
        copy(y),                        # x
        similar(y),                     # z
        zeros(Float64, m),              # alpha
        Vector{Float64}(undef, m),      # gamma
        tree_lam,
        TreeDPMem(n),
        mst,
        WeightedGraph(mst.neighbors, lam),
        tree,
        tree_alpha,
        egraph,
    )
end


"""
General framework for iterated tree optimizer:
1. Allocate memory (of type `Mem`)
2. Call `gaplas!(mem, y, graph, λ, μ)` in every iteration (`max_iter` many times).

!!! note

    `mem` must contain a `gamma` field

"""
function gaplas(
    y::Array{Float64,N},
    graph::GraphT,
    lambda::Weights{Float64},
    mu::W2,
    ::Type{Mem} = GapMem;
    max_iter::Int = 5,
    verbose::Bool = true,
)::Array{Float64,N} where {N, GraphT<:Graph, W2<:Weights{Float64}, Mem}
    mem = Mem(y, graph, lambda)
    for it in 1:max_iter
        gaplas!(mem, y, graph, lambda, mu)
        if verbose
            let gap = -sum(mem.gamma)
                @printf("%4d %12.4f\n", it, gap)
            end
        end
    end
    return mem.x
end


"""
Called in every iteration.
Implements the actual optimization, i.e.
"""
function gaplas!(
    mem::GapMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    _extract_tree!(mem, y, graph, lambda)
    tree_dp!(mem.x, mem.z, mem.tree, mem.tree_lam, mu, mem.dp_mem)
    mem.tree_alpha .= vec(mem.x) .- vec(mem.z)
    dual!(mem.tree_alpha, mem.dp_mem.proc_order, mem.mst.parent)
    update_tree!(
        mem.alpha,
        mem.tree_alpha,
        mem.mst.selected,
        mem.egraph,
        mem.mst.parent,
    )
end


function _extract_tree!(
    mem::GapMem{N, W1}, y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}
) where {N, W1<:Weights{Float64}}
    gap_vec!(mem.gamma, mem.x, mem.alpha, mem.wgraph, -1.0)
    prim_mst_edges(mem.gamma, mem.tree.root, mem.mst)
    mem.z .= y
    extract_non_tree!(
        graph,
        mem.mst.parent,
        mem.z,
        mem.alpha,
        mem.tree_lam,
        lambda,
    )
end


end
