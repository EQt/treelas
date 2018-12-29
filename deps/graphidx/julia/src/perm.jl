
"""
   inverse

Inverse a permutation.
If n > 0, fill with zeros. Can also use `invperm`
"""
function inverse(p::Vector{Int}, n::Int = 0)
    if n <= 0
        n = length(p)
    end
    q = zeros(Int, n)
    for (i, j) in enumerate(p)
        q[j] = i
    end
    q
end


"""
    concat(a, b)

TODO
"""
concat(a::Vector{T}, i::Vector{Int}) where T =
    T[a[j] for j in i if j > 0]
