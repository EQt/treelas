module LineDP

const EPS = 1e-10
const DEBUG = false

import Printf: @printf

include("event.jl")
include("queues.jl")
include("clip.jl")


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
)::Array{F,N}  where {F, N, Lam, Mu}
    n = length(y)
    @assert n == length(x)
    resize!(mem, n)
    local events::Vector{Event} = mem.events
    local lb::Vector{F} = mem.lb
    local ub::Vector{F} = x
    local pq = Ref(Range(n+1, n))
    local λ0::F = F(0.0)

    for i = 1:(n-1)
        lb[i] = clip(events, pq, +μ[i], -μ[i] * y[i] - λ0 + λ[i], Val(true))
        ub[i] = clip(events, pq, -μ[i], +μ[i] * y[i] - λ0 + λ[i], Val(false))
        λ0 = μ[i] > EPS ? λ[i] : min(λ0, λ[i])
    end

    x[n] = clip(events, pq, +μ[n], -μ[n] * y[n] - λ0 + 0, Val(true))
    DEBUG && @printf("x[%d] = %g\n", n, x[n])
    for i = n-1:-1:1
        x[i] = clamp(x[i+1], lb[i], ub[i])
    end

    return x
end


line_las!(x::Array{F,N}, y::Array{F,N}, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    line_las!(LineDPMem{F}(N), x, y, λ, µ)

line_las(y::Array{F,N}, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    line_las!(similar(y), y, λ, µ)


end
