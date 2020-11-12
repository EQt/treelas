"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!, Sol

include("broadcast.jl")


struct MomMem{N, WL<:Weights{Float64}}
    learn::Float64
    mass::Float64
    sol::Sol{N}
    sol1::Sol{N}        # solution, one step before
    sol2::Sol{N}        # solution, two steps before
    gmem::GapMem{N, WL}
    gamma::Vector{Float64}      # overload of `gmem.gamma` to be compliant in `gaplas!`
end


function MomMem(y::Array{Float64,N}, graph::Graph, lambda::Weights{Float64}) where {N}
    learn = 0.5
    mass = 0.4
    gmem = GapMem(y, graph, lambda)
    sol1 = similar(gmem.sol)
    sol1.x .= 0
    sol1.α .= 0
    sol2 = similar(gmem.sol)
    MomMem(learn, mass, gmem.sol, sol1, sol2, gmem, gmem.gamma)
end


function gaplas!(
    mem::MomMem{N, W1},
    y::Array{Float64,N},
    graph::Graph,
    lambda::Weights{Float64},
    mu::Weights{Float64},
) where {N, W1<:Weights{Float64}}
    mem.sol2 .= mem.sol1
    mem.sol1 .= mem.sol
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
    let η = mem.learn, ζ = (1 - η) * mem.mass, ι = (1 - η) * (1 - mem.mass)
        @assert η + ζ + ι ≈ 1
        @. mem.sol.x = η * mem.sol.x + ζ * mem.sol1.x + ι * mem.sol2.x
        @. mem.sol.α = η * mem.sol.α + ζ * mem.sol1.α + ι * mem.sol2.α
    end
end


end # module Momentum
