"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!


struct MomMem{N, WL<:Weights{Float64}}
    learn::Float64
    x::Array{Float64, N}
    z::Array{Float64, N}
    α::Vector{Float64}
    gmem::GapMem{N, WL}
    gamma::Vector{Float64}
end


function MomMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    m = GraphIdx.num_edges(graph)
    gmem = GapMem(y, graph, lambda)
    gamma = gmem.gamma
    x = gmem.x
    z = similar(x)
    α = similar(gmem.alpha)
    learn = 0.5
    MomMem(learn, x, z, α, gmem, gamma)
end


function gaplas!(
    mem::MomMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    mem.z .= mem.gmem.x
    mem.α .= mem.gmem.alpha
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
    let x = mem.gmem.x, α = mem.gmem.alpha, β = mem.α, learn = mem.learn
        @. x = learn * x + (1 - learn) * mem.z
        @. α = learn * α + (1 - learn) * β
    end
end


end # CycleGap
