"""
Return the Inorder traversal of a complete binary tree with given `height`.
(Root node will be `0` and placed in the middle of the array
"""
function binary_inorder(height::Integer)::Vector{Int}
    n = (1 << Int(height)) - 1
    out = Vector{Int}(undef, n)

    s = 2
    while n >= 1
        t = s >> 1
        b, a = n-1, n >> 1
        for (i, j) in enumerate(a:b)
            out[i*s - t] = j
        end
        n >>= 1
        s <<= 1
    end

    return out
end
