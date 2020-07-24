module CycleGap

import GraphIdx
import GraphIdx: Graph, Weights, WeightedGraph, EdgeGraph
import GraphIdx: PrimMstMem, prim_mst_edges, Graph, EdgeGraph, Edge
import GraphIdx.Tree: RootedTree

import TreeLas.TreeDP: TreeDPMem, tree_dp!
import TreeLas.Dual: dual!, gap_vec!, primal_from_dual!
import TreeLas.MGT: extract_non_tree!, update_tree!


function Base.collect(g::Graph)::EdgeGraph
    m = GraphIdx.num_edges(g)
    n = GraphIdx.num_nodes(g)
    edges = Vector{Edge{Int}}(undef, m)
    GraphIdx.enumerate_edges(g) do i::Int, u::Int, v::Int, ::Float64
        edges[i] = Edge((u, v))
    end
    EdgeGraph(n, edges)
end

Base.collect(g::EdgeGraph)::EdgeGraph = g


struct GapMem{N}
    x::Array{Float64,N}
    z::Array{Float64,N}
    alpha::Vector{Float64}
    gamma::Vector{Float64}
    tree_lam::Vector{Float64}
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
    tree_lam = Vector{Float64}(undef, n)
    tree_alpha = tree_lam
    egraph = collect(graph)
    mst = PrimMstMem(egraph)
    lam = Float64[lambda[i] for i=1:m]
    tree = RootedTree(root_node, mst.parent)
    GapMem(
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


function gaplas(
    y::Array{Float64,N},
    graph::GraphT,
    lambda::Weights{Float64},
    mu::W2 = GraphIdx.Ones{Float64}();
    max_iter::Int = 5,
)::Array{Float64,N} where {N, GraphT<:Graph, W2<:Weights{Float64}}
    mem = GapMem(y, graph, lambda)
    for it in 1:max_iter
        gaplas!(mem, y, graph, lambda, mu)
    end
    return mem.x
end


function gaplas!(
    mem::GapMem{N},
    y::Array{Float64,N},
    graph::GraphT,
    lambda::Weights{Float64},
    mu::W2
) where {N, GraphT<:Graph, W2<:Weights{Float64}}
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
    tree_dp!(mem.x, mem.z, mem.tree, GraphIdx.Vec(mem.tree_lam), mu, mem.dp_mem)
    mem.tree_alpha .= vec(mem.x) .- vec(mem.z)
    dual!(mem.tree_alpha, mem.dp_mem.proc_order, mem.mst.parent)
    update_tree!(mem.alpha, mem.tree_alpha, mem.mst.selected, mem.egraph, mem.mst.parent)

end


end # CycleGap
