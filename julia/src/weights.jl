struct ConstantWeights{F<:Real}
    w::F
end


(c::ConstantWeights{F})(_::Integer) where {F} = c.w


struct ArrayWeights{F<:Real}
    a::Vector{F}
end


(c::ArrayWeights{F})(i::Integer) where {F} = c.a[i]

