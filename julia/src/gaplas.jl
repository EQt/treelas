"""
# Gap Tree Lasso

Re-implement the MGT module for general Graphs having only a method to access all edges.
"""
module GapLas
include("tree_dp.jl")

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

Perform one iteration. Premis: `x = y + D'*α`
"""
function gaplas!(x::Array{Float64}, α::Vector{Float64}, γ::Vector{Float64},
                 y::Array{Float64}, g::Graph) where {Graph}
    gap_vec!(γ, x, α, g)
end


end
