import Base: BitInteger

"""
    hyperfloor(x)

Heighest power of 2 that is smaller than `x`.
# Example

```jldoctest
julia> bitstring(UInt8(123))
"01111011"
julia> bitstring(UInt8(hyperfloor(123)))
"01000000"
```
"""
hyperfloor(x::Integer) =
    Base._prevpow2(x)
