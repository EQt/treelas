module Utils

"""
    sum2(x)

Sum of squares.

```jldoctest
julia> sum2([1, 11])
122

```
"""
function sum2(x::Array{F,N})::F where{F<:Real,N}
    local s::F = zero(F)
    for xi in x
        s += xi^2
    end
    s
end


end
