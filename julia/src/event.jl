import Printf: @sprintf

"""
Change of the derivative.
"""
struct Event
    """Position (to sort the queues)"""
    x::Float64

    """Delta slope"""
    slope::Float64
end


"""
    intercept(e)

Intercept (also called *offset*); implicitly encoded in the [`Event`](@ref).
"""
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
