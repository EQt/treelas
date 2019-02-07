"""
Dynamic Programming Tree Solver
===============================

Merge the event queues within the tree by sorting arrays.
"""
module TreeDP

include("common.jl")
include("weights.jl")

import GraphIdx: ChildrenIndex, reset!, dfs_walk


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


tree_dp(y::Array{F,N}, λ::Lam, µ::Mu, t::Tree)::Array{F,N} where {F,N,Lam,Mu} =
    tree_dp!(similar(y), y, λ, µ, t)



tree_dp!(x::Array{F,N}, y::Vector{F}, λ::Lam,
         µ::Mu, t::Tree)::Array{F,N} where {F,N,Lam,Mu} =
    tree_dp!(x, y, λ, µ, t, TreeDPMem(length(y)))


function tree_dp!(x::Array{F,N}, y::Vector{F}, λ::Lam,
                  µ::Mu, t::Tree, mem::TreeDPMem)::Array{F,N} where {F,N,Lam,Mu}
    return x
end

end
