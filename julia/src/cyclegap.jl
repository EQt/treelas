"""
Test an idea about a variant of iterated gap trees:
Include the non-tree edges by rotate the capacities in the associated cycles.
"""
module CycleGap

import Printf: @printf

import GraphIdx
import GraphIdx: Graph, EdgeGraph, Edge, Weights, WeightedGraph
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx.Tree: RootedTree

import TreeLas.TreeDP: TreeDPMem, tree_dp!
import TreeLas.Dual: dual!, gap_vec!, primal_from_dual!
import TreeLas.MGT: extract_non_tree!, update_tree!
import TreeLas: Cycle
import TreeLas.Cycle: CycleBasis


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


mutable struct CycMem{N, WL<:Weights{Float64}}
    x::Array{Float64, N}
    gamma::Vector{Float64}
    gap_mem::GapMem{N, WL}
    cycles::CycleBasis
    tree_lam::Vector{Float64}
end

function CycMem(
    y::Array{Float64,N}, graph::Graph, lambda::W
) where {N, W<:Weights{Float64}}
    gap_mem = GapMem(y, graph, lambda)
    tree_lam = collect(lambda, GraphIdx.num_nodes(graph))
    return CycMem(
        gap_mem.x,
        gap_mem.gamma,
        gap_mem,
        CycleBasis(GraphIdx.num_edges(graph), GraphIdx.num_nodes(graph)),
        tree_lam,
    )
end


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


function cyclas(
    y::Array{Float64, N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64} = GraphIdx.Ones{Float64}();
    max_iter::Int = 5,
    verbose::Bool = true,
) where {N}
    gaplas(y, graph, lambda, mu, CycMem, max_iter = max_iter, verbose = verbose)
end


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


function gaplas!(
    cmem::CycMem{N, W1},
    y::Array{Float64,N},
    graph::GraphT,
    lambda::Weights{Float64},
    mu::W2,
) where {N, GraphT<:Graph, W1<:Weights{Float64}, W2<:Weights{Float64}}
    mem = cmem.gap_mem
    _extract_tree!(mem, y, graph, lambda)

    cmem.cycles = Cycle.CycleBasis(graph, mem.mst.parent)
    tlam0, tlam = Cycle.extract_rotate(cmem.cycles, lambda)
    cmem.tree_lam .= tlam
    length(tlam) < 30 && @info tlam

    tree_dp!(mem.x, mem.z, mem.tree, GraphIdx.Vec(cmem.tree_lam), mu, mem.dp_mem)
    mem.tree_alpha .= vec(mem.x) .- vec(mem.z)
    dual!(mem.tree_alpha, mem.dp_mem.proc_order, mem.mst.parent)

    fix_alpha!(mem.tree_alpha, tlam0, mem.x, mem.mst.parent)

    update_tree!(
        mem.alpha,
        mem.tree_alpha,
        mem.mst.selected,
        mem.egraph,
        mem.mst.parent,
    )
end


function fix_alpha!(alpha, lam, x, pi)
    n = length(alpha)
    n < 30 && @info "before" alpha lam x
    for i = 1:n
        if i == pi[i]
            continue
        end
        let ai = alpha[i]
            if abs(ai) > lam[i]
                alpha[i] = sign(ai) * lam[i]
                let diff = alpha[i] - ai
                    x[i] += diff
                    x[pi[i]] -= diff
                end
            end
        end
    end
    n < 30 && @info "after" alpha lam x
end


end # CycleGap
