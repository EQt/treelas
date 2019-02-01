include("root.jl")


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

Construct a new index within the tree given by `parent`.
"""
function ChildrenIndex(parent::Vector{Int}, root::Int = 0)
    idx = ChildrenIndex(length(parent))
    reset!(idx, parent, root)
    return idx
end


"""
    reset!(cidx, parent [,root])

Actually compute the index according to `parent`.
"""
function reset!(cidx::ChildrenIndex, parent::Vector{Int}, root::Int = 0)
    if root <= 0
        root = find_root(parent)
    end
    n = length(parent)
    pi = cidx.pi
    idx = cidx.idx

    @assert parent[root] == root "pi[$root] == $(pi[root])"
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
    return cidx
end
