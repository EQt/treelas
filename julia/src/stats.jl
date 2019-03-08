"""
Some statistical functions.
"""
module Stats


"""
    weighted_median(x, [w, [∂ ]])

Compute a generalized weighted order statistics.
Hereby, we minimize
```math
\\min_{t} \\sum_{i} w_i |t - x_i| + f(t)
```
whereby `∂` is the derivative of the convex function ``f`` (by default constant `0.5`.

"""
weighted_median(x::Vector{X}, w::Vector{W}, ∂::F=(x) -> W(0)) where {X,W,F} =
    weighted_median!(copy(x), copy(w), ∂)


weighted_median(x::Vector{X}) where {X} =
    weighted_median!(copy(x), ones(length(x)))


function weighted_median!(
    x::Vector{X},
    w::Vector{W},
    ∂::F = (x) -> W(0),
    pi::Vector{Int} = Int[],
) where {X, W, F <: Function}
    @assert length(x) == length(w)
    local n = length(x)
    resize!(pi, n)
    sortperm!(pi, x)
    x = x[pi]
    w = w[pi]

    # Precompute  wsum(i,j) = sum(w[i:j])
    cumsum!(w, w)
    local wsum0 = -w[end]
    wsum(i::Int) = wsum0 + ∂(x[i]) + (i == 1 ? w[1] : w[i] + w[i-1])
        
    # sum of abs "derivative" if t < x[first] (and x is sorted)
    local first::Int, last::Int = 1, length(x)
    while last - first > 1
        local mid::Int = (first + last) ÷ 2
        local mid_val = wsum(mid)
        @show mid
        if wsum(mid) < 0
            first = mid
            @show first
        else
            last = mid
            @show last
        end
    end

    @show wsum(first), wsum(last)

    if wsum(first) >= 0
        return x[first], x[first]
    elseif wsum(last) <= 0
        return x[last], x[last]
    else
        return x[first], x[last]
    end
end
    

end
