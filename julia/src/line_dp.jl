module LineDP

include("event.jl")
include("queues.jl")

import .QueueUnion: Range
import Printf: @printf

const EPS = 1e-10
const DEBUG = false

function clip(
    elements::Vector{Event},
    pq::Ref{Range},
    slope::F,
    offset::F,
    ::Val{forward},
)::F where {F,forward}
    local start::Int = pq[].start
    local stop::Int = pq[].stop
    local e::Event = elements[forward ? start : stop]::Event
    DEBUG && @printf("clip_%s: (%+g, %+.2f)\n", forward ? "f" : "b", slope, offset)
    while start <= stop && slope * e.x + offset < 0
        offset += intercept(e)
        slope += e.slope
        DEBUG && @printf(" lip_%s: (%+g, %+.2f)\n",
                         forward ? "f" : "b", slope, offset)
        e = elements[forward ? start += 1 : stop -= 1]
    end
    if abs(slope) <= EPS
        return forward ? -Inf : +Inf
    end
    local x::F = -offset/slope
    DEBUG && @printf("  ip_%s:  --> %g\n", forward ? "f" : "b", x)
    elements[forward ? start -= 1 : stop += 1] = Event(x, slope)
    pq[] = Range(start, stop)
    return x
end


line_las(y::Array{F,N}, λ::Lam, µ::Mu) where {F,N,Lam,Mu} =
    line_las!(similar(y), y, λ, µ)

function line_las!(
    x::Array{F,N},
    y::Array{F,N},
    λ::Lam,
    µ::Mu,
)::Array{F,N}  where {F, N, Lam, Mu}
    n = length(y)
    @assert n == length(x)
    local lb::Vector{F} = Vector{F}(undef, n-1)
    local ub::Vector{F} = Vector{F}(undef, n-1)
    local events = Vector{Event}(undef, 2n)
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

end
