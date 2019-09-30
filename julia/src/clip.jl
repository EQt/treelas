import .QueueUnion: Range, Queues
import Printf: @printf

const EPS = 1e-10
const DEBUG = false


function clip_front(
    elements::Vector{Event},
    pqs::Vector{Range},
    i::Int,
    slope::Float64,
    offset::Float64,
    t::Float64,
)::Float64
    pq = pqs[i]::Range
    start = pq.start
    stop = pq.stop
    e = elements[start]::Event
    while start <= stop && slope * e.x + offset < t
        offset += intercept(e)
        slope  += e.slope
        start += 1
        e = elements[start]::Event
    end
    if abs(slope) <= EPS
        return -Inf
    end
    x = (t - offset)/slope
    start -= 1
    elements[start] = Event(x, slope)
    pqs[i] = Range(start, stop)
    return x
end


function clip_back(elements::Vector{Event}, pqs::Vector{Range}, i::Int,
                   slope::Float64, offset::Float64, t::Float64)::Float64
    begin
        pq = pqs[i]::Range
        start = pq.start
        stop = pq.stop
        e = elements[stop]::Event
        while start <= stop && slope * e.x + offset > t
            offset -= intercept(e)
            slope  -= e.slope
            stop -= 1
            e = elements[stop]::Event
        end
        if abs(slope) <= EPS
            return +Inf
        end
        x = (t - offset)/slope
        stop += 1
        elements[stop] = Event(x, -slope)
        pqs[i] = Range(start, stop)
        return x
    end
end


clip_front(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I} =
    clip_front(qs.events, qs.pq, i, slope, offset, t)


clip_back(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I} =
    clip_back(qs.events, qs.pq, i, slope, offset, t)


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
