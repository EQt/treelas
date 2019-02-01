"""
Bundle functions regarding tree graphs.

TODO: Refactor in the future.
"""
module Tree
include("unionfind.jl")
include("children.jl")
include("neighbors.jl")


function signed_dfs(f::Function, tree::ChildrenIndex,
                    stack::Vector{Int} = Vector{Int}())
    sizehint!(stack, length(tree))
    push!(stack, ~root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        f(v)
        if v < 0
            v = ~v
            push!(stack, v)
            for u in tree[v]
                push!(stack, ~u)
            end
        end
    end
end


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

    @assert parent[root_node(tree)] == root_node(tree)
    signed_dfs(tree) do v::Int
        if v >= 0
            for (u, ei) in pairs[v]
                if colors[u]
                    lcas[ei] = ancestors[uf[u]]
                end
            end

            colors[v] = true
            if v != parent[v]
                unite!(uf, uf[v], uf[parent[v]])
                ancestors[uf[parent[v]]] = parent[v]
            end
        end
    end

    return lcas
end


lowest_common_ancestors(tree, parent, pairs::Vector{Tuple{Int, Int}}) =
    lowest_common_ancestors(tree, parent, NeighborIndex(length(tree), pairs))


end
