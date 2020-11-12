"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!, Sol


Broadcast.broadcasted(::typeof(identity), s::Sol{N}) where {N} = s


function Broadcast.materialize!(dest::Sol{N}, src::Sol{N}) where {N}
    # println("materialize!(::Sol, ::Sol)")
    dest.x .= src.x
    dest.α .= src.α
end    


struct MomMem{N, WL<:Weights{Float64}}
    learn::Float64
    mass::Float64
    sol::Sol{N}
    sol0::Sol{N}
    gmem::GapMem{N, WL}
    gamma::Vector{Float64}      # overload of `gmem.gamma` to be compliant in `gaplas!`
end


function MomMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    m = GraphIdx.num_edges(graph)
    gmem = GapMem(y, graph, lambda)
    gamma = gmem.gamma
    s = Sol(similar(gmem.sol.x), similar(gmem.sol.α))
    learn = 0.85
    mass = 0.5
    MomMem(learn, mass, gmem.sol, s, gmem, gamma)
end


function gaplas!(
    mem::MomMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    mem.sol0 .= mem.sol
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
    let learn = mem.learn
        @. mem.sol.x = learn * mem.sol.x + (1 - learn) * mem.sol0.x
        @. mem.sol.α = learn * mem.sol.α + (1 - learn) * mem.sol0.α
    end
end


end # module Momentum
