module LineDP

import Printf: @printf
import ..Pwl: clip, Range, Event, EPS, DEBUG
import GraphIdx: Ones, Vec, is_const

"""
Contains all memory needed for `line_las!`.
"""
struct LineDPMem{F}
    events::Vector{Event}
    lb::Vector{F}
end


LineDPMem{F}() where {F} =
    LineDPMem{F}([], [])

LineDPMem{F}(n::Integer) where {F} =
    resize!(LineDPMem{F}(), n)


function Base.resize!(mem::LineDPMem{F}, n::Integer)::LineDPMem{F} where {F}
    resize!(mem.events, 2n)
    resize!(mem.lb, n-1)
    mem
end


function line_las!(
    mem::LineDPMem{F},
    x::Array{F,N},
    y::Array{F,N},
    λ::Lam,
    µ::Mu,
)::Array{F,N} where {F, N, Lam, Mu}
    line_las!(mem, x, y, λ, μ, Val(!is_const(Mu)))
end

function line_las!(
    mem::LineDPMem{F},
    x::Array{F,N},
    y::Array{F,N},
    λ::Lam,
    µ::Mu,
    C::Val{check},
)::Array{F,N} where {F, N, Lam, Mu, check}
    n = length(y)
    @assert n == length(x)
    resize!(mem, n)
    local events::Vector{Event} = mem.events
    local lb::Vector{F} = mem.lb
    local ub::Vector{F} = x
    local pq = Ref(Range(n+1, n))
    local λ0::F = F(0.0)

    @static DEBUG && @info y
    @inbounds for i = 1:(n-1)
        lb[i] = clip(events, pq, +μ[i], -μ[i] * y[i] - λ0 + λ[i], Val(true), C)
        ub[i] = clip(events, pq, -μ[i], +μ[i] * y[i] - λ0 + λ[i], Val(false), C)
        λ0 = (!check || μ[i] > EPS) ? λ[i] : min(λ0, λ[i])
    end

    x[n] = clip(events, pq, +μ[n], -μ[n] * y[n] - λ0 + 0, Val(true), C)
    @static DEBUG && @printf("x[%d] = %g\n", n, x[n])
    @inbounds for i = n-1:-1:1
        x[i] = clamp(x[i+1], lb[i], ub[i])
    end

    return x
end

line_las!(x::Array{F,N}, y::Array{F,N}, λ::Lam, µ::Mu) where {F, N, Lam, Mu} =
    line_las!(LineDPMem{F}(N), x, y, λ, µ)

line_las(y::Array{F,N}, λ::Lam, µ::Mu = Ones{F}()) where {F, N, Lam, Mu} =
    line_las!(similar(y), y, λ, µ)

end
