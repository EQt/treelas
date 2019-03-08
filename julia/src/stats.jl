"""
Some statistical functions.
"""
module Stats


"""
    weighted_median(x, w, [∂ ])

Compute a generalized weighted order statistics.
Hereby, we minimize
```math
\\min_{t} \\sum_{i} w_i |t - x_i| + f(t)
```
whereby `∂` is the derivative of the convex function ``f``.
"""
weighted_median(x::Vector{X}, w::Vector{W}, ∂::F=(x) -> W(0.5)) where {X,W,F} =
    weighted_median!(copy(x), copy(w), ∂)


function weighted_median!(
    x::Vector{X},
    w::Vector{W},
    ∂::F = (x) -> W(0.5),
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
    wsum(i::Int, j::Int) = w[j] - w[i]
    # sum of abs "derivative" if t < x[first] (and x is sorted)
    local acc = -wsum(1, length(w))
    local first, last = 1, length(x)
    while last - first > 1
        local mid = (first + last) ÷ 2
        local mid_val = acc + 2*wsum(first, mid)
        if mid_val + ∂(x[mid]) < 0
            acc = mid_val
            first = mid
        else
            last = mid
        end
    end
    return x[first], x[last]
end
    

end
