function node_degrees(parent::Vector{Int}, root = 1)
    @assert parent[root] == root
    idx = similar(parent)
    idx .= 0
    for p in parent
        idx[p] += 1
    end
    idx[root] -= 1      # root does not have itself as children
    return idx
end

struct ChildrenIndex
    pi::Vector{Int}
    idx::Vector{Int}
end
ChildrenIndex(n::Int) =
    ChildrenIndex(Vector{Int}(undef, n), Vector{Int}(undef, n+1))


@inline Base.getindex(c::ChildrenIndex, j::Int) =
    view(c.pi, c.idx[j]:c.idx[j+1]-1)

Base.length(c::ChildrenIndex) = length(c.pi)

children_index(parent) =
    ChildrenIndex(_compute_children(parent)...)


function _compute_children(parent::Vector{T}, root::Int = 1) where T <: Integer
    @assert root == 1
    cidx = ChildrenIndex(length(parent))
    return _compute_children(cidx.pi, cidx.idx, parent, root)
end


"""
Return V, I such that V[I[j]:I[j+1]-1] are the children of j
"""
function _compute_children(pi::Vector{T}, idx::Vector{T},
                           parent::Vector{T}, root::T) where T <: Integer
    n = length(parent)
    @assert length(pi) == n
    @assert length(idx) == n+1

    idx .= 0
    for p in parent     # compute number of children
        idx[p] += 1
    end
    idx[root] -= 1      # root does not have itself as children

    acc = 2
    deg_i = 0
    deg_ii = idx[1]
    for i = 1:n
        idx[i] = acc
        acc += deg_i
        deg_i, deg_ii = deg_ii, idx[i+1]
    end
    idx[n+1] = acc
    @assert deg_ii == 0
    @assert(idx[end] + deg_i == n+1,
            "idx[$(length(idx))] + $deg_i = $(idx[end]) != $(n+1)")

    pi[root] = root       # root ==> isperm(pi) holds (root is not any child)
    for (v, p) in enumerate(parent)     # collect children values
        if v == p
            continue
        end
        pi[idx[p+1]] = v
        idx[p+1] += 1
    end
    @assert(idx[end] == n+1,
            "idx[$(length(idx))] + $deg_i = $(idx[end]) != $(n+1)")
    return pi, idx
end


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


concat(a::Vector{T}, i::Vector{Int}) where T =
    T[a[j] for j in i if j > 0]
