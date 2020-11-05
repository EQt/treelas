"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!


struct MomMem{N, WL<:Weights{Float64}}
    x::Array{Float64, N}
    gmem::GapMem{N, WL}
    gamma::Vector{Float64}
end


function MomMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    gmem = GapMem(y, graph, lambda)
    gamma = gmem.gamma
    x = gmem.x
    MomMem(x, gmem, gamma)
end


function gaplas!(
    mem::MomMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
end

end # CycleGap
