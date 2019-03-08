"""
Some statistical functions.
"""
module Stats


function permcumsum!(w::Array{W}, pi::Vector{I}) where {W, I<:Integer}
    local acc::W = 0
    for j in pi
        acc += w[j]
        w[j] = acc
    end
end



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

    # Precompute  wsum(i) = sum(-w[j] for j < i) + sum(+w[j] for j > i) + ∂(x[i])
    permcumsum!(w, pi)

    local wsum0 = -w[pi[end]]
    w0(i::Int) = i == 1 ? w[pi[1]] : w[pi[i]] - w[pi[i-1]]
    wsum(i::Int) = wsum0 + ∂(x[pi[i]]) + w[pi[i]] + (i == 1 ? 0 : w[pi[i-1]])
        
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
    # @show first, last, wsum(first), wsum(last)
    if wsum(first) + w0(first) > 0
        return x[pi[first]], x[pi[first]]
    elseif wsum(last) - w0(last) < 0
        return x[pi[last]], x[pi[last]]
    else
        return x[pi[first]], x[pi[last]]
    end
end
    

end
