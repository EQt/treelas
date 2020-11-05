include("gaplas.jl")


"""
Test an idea about a variant of iterated gap trees:
Include the non-tree edges by rotate the capacities in the associated cycles.
"""
module CycleGap

import GraphIdx
import GraphIdx: Graph, EdgeGraph, Edge, Weights, WeightedGraph
import GraphIdx: PrimMstMem, prim_mst_edges
import GraphIdx.Tree: RootedTree

import TreeLas.TreeDP: TreeDPMem, tree_dp!
import TreeLas.Dual: dual!
import TreeLas.MGT: update_tree!
import TreeLas: Cycle
import TreeLas.Cycle: CycleBasis
import TreeLas: GapLas
import TreeLas.GapLas: GapMem, gaplas


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


function GapLas.gaplas!(
    cmem::CycMem{N, W1},
    y::Array{Float64,N},
    graph::GraphT,
    lambda::Weights{Float64},
    mu::W2,
) where {N, GraphT<:Graph, W1<:Weights{Float64}, W2<:Weights{Float64}}
    mem = cmem.gap_mem
    GapLas._extract_tree!(mem, y, graph, lambda)

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
