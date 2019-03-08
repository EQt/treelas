"""
Some statistical functions.
"""
module Stats


"""
    weighted_median(x, w, [∂ ])

Compute a generalized weighted order statistics.
Hereby, we minimize
```math
\min_{t} \sum_{i} w_i |t - x_i| + f(t)
```
whereby `∂` is the derivative of the convex function ``f``.

At the moment, `w` and `x` will be altered.
"""
function weighted_median!(
    x::Vector{F},
    w::Vector{F},
    ∂::Function = (x) -> F(0.5),
) where {F}
    sort!(w, lt=(i,j) -> x[i] < x[j])
    sort!(x)

    # Precompute  wsum(i,j) = sum(w[i:j])
    cumsum!(w, w)
    wsum(i::Int, j::Int) = w[j] - w[i]
    local acc = -wum(1, length(w))
    local first, last = 1, length(x)
    while last - first > 1
        local mid = (first + last) ÷ 2
        local mid_val = acc + 2*wsum(first, mid) + f(x[mid])
        if mid_val < 0
            acc = acc + 2*wsum(first, mid)
            first = mid
        else
            last = mid
        end
    end
    return x[last], x[first]
end
    

end
