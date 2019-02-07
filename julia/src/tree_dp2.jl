"""
Dynamic Programming Tree Solver
===============================

Merge the event queues within the tree by sorting arrays.

Compared to previous versions, all weighting (λ and μ) is templated;
thus we only need one implementation and the compiler will do the
rest.

"""
module TreeDP

include("common.jl")
include("weights.jl")

import GraphIdx: ChildrenIndex, reset!, dfs_walk


"""
Set of double ended queues (DeQue) in a tree.
Initially all leaf nodes have its own queue.
Children's queues will be merged into the parent node.
"""
struct Queues
    events::Vector{Event}
    pq::Vector{Range}
    Queues(n) =
    new(Vector{Event}(undef, 2n),
        Vector{Range}(undef,  n))
end


Base.getindex(q::Queues, i) = q.pq[i]


"""
Contains all memory needed for `tree_dp!`.
"""
struct TreeDPMem
end


function TreeDPMem(n::Integer)
end


"""
    tree_dp(y, t::Tree, λ [, μ = 1.0])

Perform the dynamic programming algorithm on `y` and return the optimized `x`.

The edge weighting λ should be either a constant (e.g. `Float64`) or a
callable such that `λ(i)` returns the weight of the edge `(i, t.parent[i])`;
for `λ(t.root)` it might return anything but not causean error.
"""
tree_dp(y::Array{F,N}, t::Tree, λ::F, µ::F = F(1.0))::Array{F,N} where {F,N} =
    tree_dp!(similar(y), y, t, ConstantWeights(F(λ)), ConstantWeights(F(µ)))


tree_dp(y::Array{F,N}, t::Tree, λ::Lam, µ::Mu)::Array{F,N} where {F,N,Lam,Mu} =
    tree_dp!(similar(y), y, t, λ, µ)


"""
    tree_dp!(x, y, t, λ, μ [,mem])

Like `tree_dp` but do not allocate an output buffer for `x`.

If `mem::TreeDPMem` is provided, no additional allocations will be
needed.
"""
tree_dp!(x::Array{F,N}, y::Vector{F}, t::Tree, λ::Lam,
         µ::Mu)::Array{F,N} where {F,N,Lam,Mu} =
    tree_dp!(x, t, y, λ, µ, TreeDPMem(length(y)))


function tree_dp!(x::Array{F,N}, y::Array{F,N}, t::Tree, λ::Lam,
                  µ::Mu, mem::TreeDPMem)::Array{F,N} where {F,N,Lam,Mu}
    return x
end


end
