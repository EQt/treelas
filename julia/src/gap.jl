import GraphIdx.LinA: IncMat
import GraphIdx: enumerate_edges


"""
    gap_vec!(γ, dif, x, y, D, Dt, α, c)

Compute the gap vector (modifies `dif` and `x`) and stores it in `γ`.

!!! warning
    This is the old method using Julia's `SparseArrays.SparseMatrixCSC`.

"""
function gap_vec!(γ::Vector{Float64},
                  dif::Vector{Float64},
                  x::Vector{Float64},
                  y::Vector{Float64},
                  D::IncMat,
                  Dt::IncMat,
                  α::Vector{Float64},
                  c::Float64 = Float64(1.0))
    mul!(x, Dt, α)
    x .-= y
    mul!(dif, D, x)
    γ .= c .* ((α .* dif) .+ abs.(dif))
    return γ
end


"""
    gap_vec!(γ, x, α, g [, c = 1.0])

Compute the duality gap: for each edge `e` we set `γ[e]` to the
difference of the edge cost ``\\lambda_{ij} |x_i - x_j|`` and the
linear approximation via the dual ``α_{ij}(x_i - x_j)``.

The graph `g` needs to implement a method `enumerate_edges(::Function, g)`.

If provided, the result is multiplied by `c`.
"""
function gap_vec!(γ::Vector{F},
                  x::Array{F},
                  α::Vector{F},
                  g::Graph,
                  c::F = F(1.0)) where {F,Graph}
    enumerate_edges(g) do ei::Int, u::Int, v::Int, lam::Float64
        let diff = x[u] - x[v]
            γ[ei] = c * (lam*abs(diff) - α[ei] * diff)
        end
    end
end
