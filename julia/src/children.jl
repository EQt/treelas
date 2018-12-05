function compute_children2(parent::Vector{T}, root::Int = 1) where T <: Integer
    @assert root == 1
    n = length(parent)
    pi = Vector{T}(n)

    idx = zeros(T, n+1)
    for p in parent     # compute number of children
        idx[p] += 1
    end
    deg_i = idx[root]
    idx[root] -= 1      # root does not have itself as children
    pos = 1
    for i = 1:n         # accumulate degrees before ==> idx
        pos += deg_i
        deg_i = idx[i+1]
        idx[i+1] = pos
    end
    # idx - idx_


    pi[root] = 1        # root ==> isperm(pi) holds (root is not any child)
    found = copy(idx)   # position where next child is inserted
    for (v, p) in enumerate(parent)     # collect children values
        if v == p
            continue
        end
        pi[found[p]] = v
        found[p] += 1
    end

    return pi, idx
end


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
ChildrenIndex(n::Int) = ChildrenIndex(Vector{Int}(n), Vector{Int}(n+1))


@inline Base.getindex(c::ChildrenIndex, j::Int) =
    view(c.pi, c.idx[j]:c.idx[j+1]-1)

Base.length(c::ChildrenIndex) = length(c.pi)

children_index(parent) = ChildrenIndex(compute_children3(parent)...)


function compute_children3(parent::Vector{T}, root::Int = 1) where T <: Integer
    @assert root == 1
    cidx = ChildrenIndex(length(parent))
    return _compute_children(cidx.pi, cidx.idx, parent, root)
end


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



"""Return V, I such that V[I[j]:I[j+1]-1] are the children of j"""
function compute_children(parent::Vector{T}) where T
    n = length(parent)
    pi = sortperm(parent)
    # Sort s.t. nodes having the same parent (sisters) are neighbored.
    # In other words: collect(zip(pi, parent[pi])) will contain
    # exactly the (child, parent) but with the right column sorted!
    idx = fill(n+1, n+1)
    k = 1
    for i in 2:n
        while parent[pi[i]] >= k
            idx[k] = i
            k += 1
        end
    end
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
