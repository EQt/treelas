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
whereby `∂` is the derivative of the convex function ``f``
(by default constant `0` for ordinary weighted median).

Return the interval ``[a, b]`` containing the minimizer (if unique ``a = b``).
"""
weighted_median(x::Vector{X}, w::Vector{W}, ∂::F= _ -> W(0)) where {X, W, F} =
    weighted_median!(copy(x), copy(w), ∂)


weighted_median(x::Vector{X}) where {X} =
    weighted_median!(copy(x), ones(length(x)))


function weighted_median!(
    x::Vector{X},
    w::Vector{W},
    ∂::F = _ -> W(0),
    pi::Vector{Int} = Int[],
)::Tuple{X,X} where {X, W, F <: Function}
    @assert length(x) == length(w)
    local n = length(x)
    resize!(pi, n)
    sortperm!(pi, x)
    x = x[pi]
    w = w[pi]

    # Precompute  wsum(i) = sum(-w[j] for j < i) + sum(+w[j] for j > i) + ∂(x[i])
    cumsum!(w, w)
    local wsum0 = -w[end]
    wsum(i::Int) = wsum0 + ∂(x[i]) + w[i] + (i == 1 ? 0 : w[i-1])
        
    # sum of abs "derivative" if t < x[first] (and x is sorted)
    local first::Int, last::Int = 1, length(x)
    while last - first > 1
        local mid::Int = (first + last) ÷ 2
        if wsum(mid) < 0
            first = mid
        else
            last = mid
        end
    end

    if wsum(first) >= 0
        return x[first], x[first]
    elseif wsum(last) <= 0
        return x[last], x[last]
    else
        return x[first], x[last]
    end
end
    

end
