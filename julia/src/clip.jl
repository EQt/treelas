import .QueueUnion: Range, Queues
import Printf: @printf

const EPS = 1e-10
const DEBUG = false


@deprecate(
    clip_front(elements, pqs, i, slope, offset, t),
    clip(elements, Ref(pqs, i), +slope, +offset - t, Val(true))
)


@deprecate(
    clip_back(elements, pqs, i, slope, offset, t),
    clip(elements, Ref(pqs, i), -slope, -offset + t, Val(false))
)


@deprecate(
    clip_front(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I},
    clip(qs, i, slope, offset - t, Val(true))
)


@deprecate(
    clip_back(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I},
    clip(qs, i, -slope, -offset + t, Val(false))
)


clip(qs::Queues{Event}, i::I, slope::F, offset::F, v::Val{D}) where {F,I,D} =
    clip(qs.events, Ref(qs.pq, i), slope, offset, v)

function clip(
    elements::Vector{Event},
    pq::Ref{Range},
    slope::F,
    offset::F,
    ::Val{forward},
    ::Val{check} = Val(true),
)::F where {F,forward,check}
    local start::Int = pq[].start
    local stop::Int = pq[].stop
    local e::Event = elements[forward ? start : stop]::Event
    @static DEBUG && local dir::String = forward ? "f" : "b"
    @static DEBUG && @printf("clip_%s: (%+g, %+.2f)\n", dir, slope, offset)
    while start <= stop && slope * e.x + offset < 0
        offset += intercept(e)
        slope += e.slope
        @static DEBUG && @printf(" lip_%s: (%+g, %+.2f)\n", dir, slope, offset)
        e = elements[forward ? start += 1 : stop -= 1]
    end
    local x::F = if check && abs(slope) <= EPS
        forward ? -Inf : +Inf
    else
        let x::F = -offset/slope
            elements[forward ? start -= 1 : stop += 1] = Event(x, slope)
            x
        end
    end
    @static DEBUG && @printf("  ip_%s:  --> %g\n", dir, x)
    pq[] = Range(start, stop)
    return x
end
