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
import TreeLas.GapLas: GapMem, gaplas, Sol


"""
Memory to compute an iteration using the cycle shift idea.

!!! note

    At the moment, CyclceBasis does not have a `reset!` method or alike, so this has to be
    `mutable struct`

"""
mutable struct CycMem{N, WL<:Weights{Float64}}
    sol::Sol{N}
    gamma::Vector{Float64}
    gap_mem::GapMem{N, WL}
    cycles::CycleBasis
    λ_tree::Vector{Float64}
end


function CycMem(y::Array{Float64,N}, graph::Graph, lambda::W) where {N, W<:Weights{Float64}}
    gap_mem = GapMem(y, graph, lambda)
    λ_tree = collect(lambda, GraphIdx.num_nodes(graph))
    return CycMem(
        gap_mem.sol,
        gap_mem.gamma,
        gap_mem,
        CycleBasis(GraphIdx.num_edges(graph), GraphIdx.num_nodes(graph)),
        λ_tree,
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


function GapLas.gaplas!(
    cmem::CycMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    mem = cmem.gap_mem
    GapLas.find_gap_tree!(mem, y, graph, lambda)

    cmem.cycles = Cycle.CycleBasis(graph, mem.mst.parent)
    tlam0, tlam = Cycle.extract_rotate(cmem.cycles, lambda)
    cmem.λ_tree .= tlam
    length(tlam) < 30 && @debug tlam

    tree_dp!(mem.sol.x, mem.y_tree, mem.tree, GraphIdx.Vec(cmem.λ_tree), mu, mem.dp_mem)
    mem.α_tree .= vec(mem.sol.x) .- vec(mem.y_tree)
    dual!(mem.α_tree, mem.dp_mem.proc_order, mem.mst.parent)

    fix_alpha!(mem.α_tree, tlam0, mem.sol.x, mem.mst.parent)

    update_tree!(
        mem.sol.α,
        mem.α_tree,
        mem.mst.selected,
        mem.egraph,
        mem.mst.parent,
    )
end


function fix_alpha!(alpha, lam, x, pi)
    n = length(alpha)
    n < 30 && @debug "before" alpha lam x
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
    n < 30 && @debug "after" alpha lam x
end


end # CycleGap
