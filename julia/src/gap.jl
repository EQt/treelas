import GraphIdx.LinA: IncMat
import GraphIdx: enumerate_edges, WeightedGraph
import GraphIdx.Tree: WeightedTree
import GraphIdx.Grid: GridGraph


"""
    gap_vec!(γ, x, α, g [, c = 1.0])

Compute the duality gap: for each edge `e` we set `γ[e]` to the
difference of the edge cost ``\\lambda_{ij} |x_i - x_j|`` and the
linear approximation via the dual ``α_{ij}(x_i - x_j)``.

The graph `g` needs to implement a method `enumerate_edges(::Function, g)`.

If provided, the result is multiplied by `c`.
"""
function gap_vec!(
    γ::Vector{F},
    x::Array{F},
    α::Vector{F},
    g::Union{WeightedGraph, WeightedTree},
    c::F = F(1.0),
) where {F<:Real}
    enumerate_edges(g) do ei::Int, u::Int, v::Int, lam::Float64
        let diff = x[u] - x[v]
            γ[ei] = c * (lam*abs(diff) + α[ei] * diff)
        end
    end
end

function gap_vec!(
    γ::Vector{F},
    x::Array{F},
    α::Vector{F},
    g::GridGraph,
    c::F = F(1.0),
) where {F<:Real}
    enumerate_edges(g, weighted=true) do ei::Int, u::Int, v::Int, lam::Float64
        let diff = x[u] - x[v]
            γ[ei] = c * (lam*abs(diff) + α[ei] * diff)
        end
    end
end



"""
    gap_vec!(γ, dif, x, y, D, Dt, α, c)

Compute the gap vector (modifies `dif` and `x`) and stores it in `γ`.

!!! warning
    This is the old method using Julia's `SparseArrays.SparseMatrixCSC`.

"""
function gap_vec!(
    γ::Vector{Float64},
    dif::Vector{Float64},
    x::Vector{Float64},
    y::Vector{Float64},
    D::IncMat,
    Dt::IncMat,
    α::Vector{Float64},
    c::Float64 = Float64(1.0),
)
    mul!(x, Dt, α)
    x .-= y
    mul!(dif, D, x)
    γ .= c .* ((α .* dif) .+ abs.(dif))
    return γ
end


"""
    wolfe_gap_step(x0, α0, x1, α1)

Optimal gap step width according to Wolfe's condition, i.e.
compute the optimal step width ``θ ∈ [0, 1]`` to walk
from `(x0, α0)` to `(x1, α1)` such that the `sum(gap_vec)` will be minimized.
"""
function wolfe_gap_step(
    x0::Array{F},
    α0::Vector{F},
    x1::Array{F},
    α1::Vector{F},
    g::Graph
)::F where {F<:Real,Graph}
    θ::F = 0
    m = num_edges(g)
    d0 = Vector{F}(undef, m)
    d1 = Vector{F}(undef, m)
    enumerate_edges(g) do ei::Int, u::Int, v::Int, lam::Float64
        d0[ei] = x0[u] - x0[v]
        d1[ei] = x1[u] - x1[v]
    end
    error("todo: not implemented, yet")
    return θ
end
