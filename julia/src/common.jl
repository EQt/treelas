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

