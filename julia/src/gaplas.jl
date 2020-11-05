"""
Re-factoring and re-implementation of MGT
"""
module GapLas

import GraphIdx
import GraphIdx: Graph, EdgeGraph, Edge, Weights, WeightedGraph
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx.Tree: RootedTree
import TreeLas.TreeDP: TreeDPMem, tree_dp!


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


end
