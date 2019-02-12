import Printf: @sprintf

# Event ------------------------------------------------------------------
struct Event
    x::Float64      # position (to sort the queues)
    slope::Float64  # delta slope
end

intercept(e::Event)::Float64 = -e.x * e.slope

Base.isless(e1::Event, e2::Event) = isless(e1.x, e2.x)

Base.zero(::Type{Event}) = Event(NaN, NaN, NaN)

Base.isnan(e::Event) = isnan(e.x)

Base.show(io::IO, e::Event) =
    print(io,
          if isnan(e) "Event(NaN)" else
              @sprintf("% .2f -> % .2ft + % .3f",
                       e.x, e.slope, intercept(e))
          end)

# Queues ----------------------------------------------------------------
"""
    Range(start, stop)

A range in a `Vector` containing the `Event`s of a nodes "queue".

ATTENTION: unlike in Python, `start` and `stop` are inclusive!
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
