import .QueueUnion: Range, Queues
import Printf: @printf

const EPS = 1e-10
const DEBUG = false


@deprecate(clip_front(elements, pqs, i, slope, offset, t),
           clip(elements, Ref(pqs, i), +slope, +offset - t, Val(true)))


@deprecate(clip_back(elements, pqs, i, slope, offset, t),
           clip(elements, Ref(pqs, i), -slope, -offset + t, Val(false)))



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
