"""
Bundle functions regarding tree graphs.

TODO: Refactor in the future.
"""
module Tree
include("unionfind.jl")
include("children.jl")
include("neighbors.jl")


"""
    lowest_common_ancestors(tree, lcas)



Compare to
`networkx/algorithms/lowest_common_ancestors.py`

[1]: https://en.wikipedia.org/wiki/Lowest_common_ancestor
"""
function lowest_common_ancestors(tree::ChildrenIndex,
                                 parent::Vector{Int},
                                 pairs::NeighborIndex)::Vector{Int}
    k = num_edges(pairs)
    n = num_nodes(tree)
    uf = UnionFind(n)

    colors = fill(false, n)
    ancestors = fill(-1, k)

    stack = Vector{Int}()
    sizehint!(stack, n)
    push!(stack, root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        colors[v] = true
        for u in pairs[v]:
            if colors[u]
                # set ancestors([u, v])
            end
        end
        for u in tree[v]
            push!(stack, u)
        end
        if v != parent[v]
            unite!(uf, find(v), find(parent[v]))
        end
    end

    return ancestors
end


lowest_common_ancestors(tree, pairs::Vector{Tuple{Int, Int}}) =
    lowest_common_ancestors(tree, NeighborIndex(length(tree), pairs))


end
