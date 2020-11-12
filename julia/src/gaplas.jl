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


struct Sol{N}
    x::Array{Float64, N}
    α::Vector{Float64}
end


struct GapMem{N, WL<:Weights{Float64}}
    x::Array{Float64,N}
    alpha::Vector{Float64}
    y_tree::Array{Float64,N}
    α_tree::Vector{Float64}
    λ_tree::WL
    gamma::Vector{Float64}
    wgraph::WeightedGraph
    egraph::EdgeGraph
    dp_mem::TreeDPMem
    mst::PrimMstMem
    tree::RootedTree
end


function GapMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    @assert length(y) == GraphIdx.num_nodes(graph)
    root_node = 1
    m = GraphIdx.num_edges(graph)
    n = GraphIdx.num_nodes(graph)
    λ_tree = similar(lambda, n)
    α_tree = Vector{Float64}(undef, n)
    egraph = collect(graph)
    mst = PrimMstMem(egraph)
    lam = Float64[lambda[i] for i=1:m]
    tree = RootedTree(root_node, mst.parent)
    return GapMem(
        copy(y),                        # x
        zeros(Float64, m),              # alpha
        similar(y),                     # y_tree
        α_tree,
        λ_tree,
        Vector{Float64}(undef, m),      # gamma
        WeightedGraph(mst.neighbors, lam),
        egraph,
        TreeDPMem(n),
        mst,
        tree,
    )
end


"""
General framework for iterated tree optimizer:
1. Allocate memory (of type `Mem`)
2. Call `gaplas!(mem, y, graph, λ, μ)` in every iteration (`max_iter` many times).

!!! note

    `mem` must contain a `gamma` and an `x` field

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

Optional: node weights `μ` are set to `GraphIdx.Ones{Float64}`
"""
function gaplas(
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    ::Type{Mem} = GapMem;
    max_iter::Int = 5,
    verbose::Bool = true,
)::Array{Float64,N} where {N, Mem}
    gaplas(
        y, graph, lambda, GraphIdx.Ones{Float64}(), Mem; max_iter=max_iter, verbose=verbose
    )
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
    find_gap_tree!(mem, y, graph, lambda)
    tree_dp!(mem.x, mem.y_tree, mem.tree, mem.λ_tree, mu, mem.dp_mem)
    mem.α_tree .= vec(mem.x) .- vec(mem.y_tree)
    dual!(mem.α_tree, mem.dp_mem.proc_order, mem.mst.parent)
    update_tree!(
        mem.alpha,
        mem.α_tree,
        mem.mst.selected,
        mem.egraph,
        mem.mst.parent,
    )
end


"""
Determine a tree by choosing edges with high gap value.
"""
function find_gap_tree!(
    mem::GapMem{N, W1}, y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}
) where {N, W1<:Weights{Float64}}
    gap_vec!(mem.gamma, mem.x, mem.alpha, mem.wgraph, -1.0)
    prim_mst_edges(mem.gamma, mem.tree.root, mem.mst)
    mem.y_tree .= y
    extract_non_tree!(
        graph,
        mem.mst.parent,
        mem.y_tree,
        mem.alpha,
        mem.λ_tree,
        lambda,
    )
end


end
