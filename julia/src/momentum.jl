"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!, Sol


struct MomMem{N, WL<:Weights{Float64}}
    learn::Float64
    mass::Float64
    x::Array{Float64, N}        # overload of `gmem.x` to be compliant in `gaplas!`
    s::Sol{N}
    gmem::GapMem{N, WL}
    gamma::Vector{Float64}      # overload of `gmem.gamma` to be compliant in `gaplas!`
end


function MomMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    m = GraphIdx.num_edges(graph)
    gmem = GapMem(y, graph, lambda)
    gamma = gmem.gamma
    x = gmem.x
    z = similar(x)
    α = similar(gmem.alpha)
    s = Sol(z, α)
    learn = 0.85
    mass = 0.5
    MomMem(learn, mass, x, s, gmem, gamma)
end


function gaplas!(
    mem::MomMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    mem.s.x .= mem.gmem.x
    mem.s.α .= mem.gmem.alpha
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
    let x = mem.gmem.x, α = mem.gmem.alpha, α0 = mem.s.α, learn = mem.learn, x0 = mem.s.x
        @. x = learn * x + (1 - learn) * x0
        @. α = learn * α + (1 - learn) * α0
    end
end


end # module Momentum
