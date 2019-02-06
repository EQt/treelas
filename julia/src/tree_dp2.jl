"""
Dynamic Programming Tree Solver
===============================

GOAL: avoid reallocation of memory as much as possible.
"""
module DPTree

import GraphIdx: ChildrenIndex, reset!, dfs_walk


struct ConstantWeights{F}
    w::F
end


@inline (c::ConstantWeights{F})(_::Int)::F where {F} =
    c.w


struct ArrayWeights{F}
    a::Array{F}
end


@inline (c::ConstantWeights{F})(i::Int)::F where {F} =
    c.a[i]




function tree_dp(y::Vector{F}, λ::Lam, µ::Mu, t::Tree) where {F, Lam, Mu}
end
