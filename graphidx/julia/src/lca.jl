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
    lcas = fill(-1, k)
    ancestors = collect(Int, 1:n)

    stack = Vector{Int}()
    sizehint!(stack, n)
    push!(stack, root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        for u in tree[v]
            push!(stack, u)
        end

        colors[v] = true
        for (u, ei) in pairs[v]
            @show (u, ei)
            if colors[u]
                lcas[ei] = ancestors[find(uf, u)]
            end
        end
            if v != parent[v]
                unite!(uf, find(uf, v), find(uf, parent[v]))
                ancestors[find(uf, parent[v])] = parent[v]
            end
    end

    return lcas
end


lowest_common_ancestors(tree, parent, pairs::Vector{Tuple{Int, Int}}) =
    lowest_common_ancestors(tree, parent, NeighborIndex(length(tree), pairs))


end
