"""
Dynamic Programming Tree Solver
===============================

GOAL: avoid reallocation of memory as much as possible.
"""
module TreeDP

include("common.jl")
include("weights.jl")

import GraphIdx: ChildrenIndex, reset!, dfs_walk


tree_dp(y::Array{F}, λ::Lam, µ::Mu, t::Tree) where {F, Lam, Mu} =
    tree_dp!(similar(y), y, λ, µ, t) 


struct Queues
    events::Vector{Event}
    pq::Vector{Range}
end

Queues(n) =
    new(Vector{Event}(undef, 2n),
        Vector{Range}(undef,  n))

Base.getindex(q::Queues, i) = q.pq[i]


struct TreeDPMem
end


function TreeDPMem(n::Integer)
end


function tree_dp!(x::Vector{F}, y::Vector{F}, λ::Lam, µ::Mu, t::Tree) where {F, Lam, Mu}
end


end
