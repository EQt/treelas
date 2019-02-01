struct ChildrenIndex
    pi::Vector{Int}
    idx::Vector{Int}
end


@inline Base.getindex(c::ChildrenIndex, j::Int) =
    view(c.pi, c.idx[j]:c.idx[j+1]-1)


num_nodes(c::ChildrenIndex) = length(c.pi)

root_node(c::ChildrenIndex) = c.pi[1]

Base.length(c::ChildrenIndex) = num_nodes(c)


"""
    ChildrenIndex(n::Int)

Allocate enough space for a tree with `n` nodes.
"""
ChildrenIndex(n::Int) =
    ChildrenIndex(Vector{Int}(undef, n), Vector{Int}(undef, n+1))


"""
    ChildrenIndex(parent, [root])

Initialize enough space for a tree with `n` nodes.
"""
ChildrenIndex(pi::Vector{Int}, root::Int = 1) =
    ChildrenIndex(_compute_children(pi, root)...)


"""
    _compute_children([pi, idx,], parent, [root])

Actually compute the index structure (implementation).
If not provided, the output vectors `pi` and `idx` will be allocated.
"""
_compute_children(parent::Vector{Int}, root::Int = 1) =
    let n = length(parent)
        _compute_children(Vector{Int}(undef, n),
                          Vector{Int}(undef, n+1),
                          parent,
                          root)
    end


function _compute_children(pi::Vector{Int},
                           idx::Vector{Int},
                           parent::Vector{Int},
                           root::Int)
    n = length(parent)

    @assert pi[root] == root "pi[$root] == $(pi[root])"
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
