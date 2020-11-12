"""
Implement a variant of MGT using momentum accelaration.

*Experimental*
"""
module Momentum
import GraphIdx
import GraphIdx: Graph, Weights
import TreeLas: GapLas
import TreeLas.GapLas: gaplas, GapMem, gaplas!, Sol


"""
Compute convex combination of `a` and `b` using factor `c` and store it in `out`.
"""
function conv!(out::Sol{N}, a::Sol{N}, c::Number, b::Sol{N}) where {N}
    @. out.x = c * a.x + (1 - c) * b.x
    @. out.α = c * a.α + (1 - c) * b.α
end


conv!(
    out::Sol{N}, a::Sol{N}, c::Number, b::Tuple{Array{Float64,N}, Vector{Float64}}
) where {N} =
    conv!(out, a, c, Sol(b[1], b[2]))


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
    mem.sol0.x .= mem.sol.x
    mem.sol0.α .= mem.sol.α
    GapLas.gaplas!(mem.gmem, y, graph, lambda, mu)
    let learn = mem.learn
        @. mem.sol.x = learn * mem.sol.x + (1 - learn) * mem.sol0.x
        @. mem.sol.α = learn * mem.sol.α + (1 - learn) * mem.sol0.α
    end
end


end # module Momentum
