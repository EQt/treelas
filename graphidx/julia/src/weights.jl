"""
Represent a constant weight that is indexable and callable, e.g.
```jldoctest
julia> w = ConstantWeights(5.4); w(3)
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



struct ArrayWeights{F<:Real}
    a::Vector{F}
end


(c::ArrayWeights{F})(i::Integer) where {F} = c.a[i]

Base.getindex(c::ArrayWeights{F}, i::Integer) where {F} = c.a[i]

