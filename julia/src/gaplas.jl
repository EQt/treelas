"""
# Gap Tree Lasso

Re-implement the MGT module for general Graphs having only a method to access all edges.
"""
module GapLas
include("tree_dp.jl")

import GraphIdx.Grid: iter_edges


"""
    gaplas(...)
TODO
"""
function gaplas(y::Array{Float64}, g::Graph) where {Graph}
    x = copy(y)
    α = zeros(num_edges(g))
    γ = Vector{Float64}(undef, num_edges)
    gaplas!(x, α, γ, y, g)
end


function gap_vec!(γ, x, α, g)
    ei = Int(1)
    iter_edges(g) do u::Int, v::Int, lam::Float64
        γ[ei] = (lam - abs(α[ei])) * (x[u] - x[v])
        ei += 1
    end
end


"""
    gaplas!(...)

Perform one iteration. Premises:
1. `x = y + D'*α`
2. `α` dually feasible, `abs(α[e]) ≤ λ[e]` for all edges `e`.
"""
function gaplas!(x::Array{Float64}, α::Vector{Float64}, γ::Vector{Float64},
                 y::Array{Float64}, g::Graph) where {Graph}
    gap_vec!(γ, x, α, g)
end


end
