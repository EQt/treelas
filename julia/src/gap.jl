import GraphIdx.LinA: IncMat


"""
    gap_vec!(...)

Compute the gap vector (modifies dif and x) and stores it in γ.
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

