"""
# Gap Tree Lasso

Re-implement the MGT module for general Graphs having only a method to
access all edges.  One crucial part question is how to provide access
to a `NeighborIndex` or alike.
"""
module GapLas

import GraphIdx.Grid: enumerate_edges
import GraphIdx: NeighborIndex, neighbors_lambda
import GraphIdx: PrimMstMem, prim_mst_edges
import ..TreeDP: TreeDPMem, tree_dp!, reset!


"""
    gaplas(...)

Graph has to implement several methods:
- `iter_edges(::Function, ::Graph)`
- `NeighborIndex(::Graph)`
"""
function gaplas(y::Array{Float64}, g::Graph) where {Graph}
    x = copy(y)
    α = zeros(num_edges(g))
    γ = Vector{Float64}(undef, num_edges)
    gaplas!(x, α, γ, y, g)
end


function gap_vec!(γ::Vector{F},
                  x::Array{F},
                  α::Vector{F},
                  g::Graph,
                  c::F = F(1.0)) where {F,Graph}
    enumerate_edges(g) do ei::Int, u::Int, v::Int, lam::Float64
        γ[ei] = c * (lam - abs(α[ei])) * (x[u] - x[v])
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
