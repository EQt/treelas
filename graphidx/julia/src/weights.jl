"""
Represent weighting that is indexable and callable where each index has the same value.

Usually, weights are assumed to be positive!

## Example
```jldoctest
julia> w = GraphIdx.ConstantWeights(5.4);

julia> w(3)
5.4

julia> w[3]
5.4
```
"""
struct ConstantWeights{F<:Real}
    w::F
end


(c::ConstantWeights{F})(_::Integer) where {F} = c.w

Base.getindex(c::ConstantWeights{F}, _::Integer) where {F} = c.w



"""
Weighting of nodes where every weight can be different.
The actual value of a node can be accessed via call - or index syntax.

## Example
```jldoctest
julia> w = GraphIdx.ArrayWeights([0.1, 0.2, 0.3]);

julia> w(1)
0.1

julia> w[3]
0.3
```
"""
struct ArrayWeights{F<:Real}
    a::Vector{F}
end


(c::ArrayWeights{F})(i::Integer) where {F} = c.a[i]

Base.getindex(c::ArrayWeights{F}, i::Integer) where {F} = c.a[i]
