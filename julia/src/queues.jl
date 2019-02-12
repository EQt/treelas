include("event.jl")


"""
    Range(start, stop)

A range in a `Vector` containing the `Event`s of a nodes "queue".

!!! warning

    unlike in Python, `start` and `stop` are inclusive!
"""
struct Range
    start::Int
    stop::Int
end

Base.length(q::Range) = q.stop - q.start + 1

Base.show(io::IO, q::Range) = print(io, q.start, ":", q.stop)


function clip_front(elements::Vector{Event}, pqs::Vector{Range}, i::Int,
                    slope::Float64, offset::Float64, t::Float64)::Float64
    begin
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
        x = (t - offset)/slope
        start -= 1
        elements[start] = Event(x, slope)
        pqs[i] = Range(start, stop)
        return x
    end
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
        x = (t - offset)/slope
        stop += 1
        elements[stop] = Event(x, -slope)
        pqs[i] = Range(start, stop)
        return x
    end
end


function merge(elements::Vector{Event}, parent::Range, child::Range)::Range
    if parent.start <= parent.stop
        gap = child.start - parent.stop - 1
        old_stop = parent.stop
        res = Range(parent.start, child.stop - gap)
        if gap > 0
            for i in old_stop+1:res.stop
                elements[i] = elements[i+gap]
            end
        end
        sort!(elements, Int(res.start), Int(res.stop),
              PartialQuickSort(Int(res.start):Int(res.stop)),
              Base.Sort.Forward)
        return res
    end
    return child
end


"""
Set of double ended queues (DeQue) in a tree.
Initially all leaf nodes have its own queue.
Children's queues will be merged into the parent node.
"""
struct Queues{E}
    events::Vector{E}
    pq::Vector{Range}
end

Queues(n::Integer, E::Type = Event) =
    Queues{E}(Vector{E}(undef, 2n),
              Vector{Range}(undef,  n))


Base.getindex(q::Queues, i) = q.pq[i]


clip_front(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I} =
    clip_front(qs.events, qs.pq, i, slope, offset, t)


clip_back(qs::Queues{Event}, i::I, slope::F, offset::F, t::F) where {F,I} =
    clip_back(qs.events, qs.pq, i, slope, offset, t)


function merge!(qs::Queues{E}, i::I, j::I) where {I,E}
    qs.pq[i] = merge(qs.events, qs.pq[i], qs.pq[j])
end
