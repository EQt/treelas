include("root.jl")

"""
Provide constant time access to children of a node by index operator.
By convention, the root node is stored at ChildrenIndex.idx[1].
"""
struct ChildrenIndex
    idx::Vector{Int}
    value::Vector{Int}
end


@inline Base.getindex(c::ChildrenIndex, j::Int) =
    view(c.value, c.idx[j]:c.idx[j+1]-1)


num_nodes(c::ChildrenIndex) = length(c.value)

root_node(c::ChildrenIndex) = c.value[1]

Base.length(c::ChildrenIndex) = num_nodes(c)


"""
    ChildrenIndex(n::Int)

Allocate enough space for a tree with `n` nodes.
"""
ChildrenIndex(n::Int) =
    ChildrenIndex(Vector{Int}(undef, n+1), Vector{Int}(undef, n))


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
    value = cidx.value
    idx = cidx.idx

    @assert parent[root] == root "value[$root] == $(value[root])"
    @assert length(value) == n
    @assert length(idx) == n+1

    idx .= 0
    for p in parent     # compute number of children
        idx[p] += 1
    end
    idx[root] -= 1      # root does not have itself as children

    value[1] = root     # root ==> isperm(value) holds (root is not any child)
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

    for (v, p) in enumerate(parent)     # collect children values
        if v == p
            continue
        end
        value[idx[p+1]] = v
        idx[p+1] += 1
    end
    @assert(idx[end] == n+1,
            "idx[$(length(idx))] + $deg_i = $(idx[end]) != $(n+1)")
    return cidx
end
