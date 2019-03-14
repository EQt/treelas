import ..Utils: UnionFind, find, unite!


"""
    lowest_common_ancestors(tree, parent, pairs)

For a list of pairs `[(u1, v1), ..., (uk, vk)]` compute the
[lowest common ancestors](https://en.wikipedia.org/wiki/Lowest_common_ancestor)
`[a1, ..., ak]`, i.e. `ai` is the lowest node in the tree that is
ancestor (parent, grand-parent, grand-grand-parent, etc.) of both, `ui` and `vi`.

Runtime is `O(length(tree) + length(pairs))`.
More precisely, due to the use of the
[`UnionFind`](@ref) data structure, the inverse Ackermann function is also included;
however the impact is at most 4 for all practical instances (``n < 10^{100}``).

See CLRS (3rd edition) page 584 or
[`networkx`](https://github.com/networkx/networkx/blob/master/networkx/algorithms/lowest_common_ancestors.py).
"""
function lowest_common_ancestors(
    tree::ChildrenIndex,
    parent::Vector{Int},
    pairs::IncidenceIndex,
)::Vector{Int}
    @assert parent[root_node(tree)] == root_node(tree)

    local k = num_edges(pairs)
    local n = num_nodes(tree)
    local uf = UnionFind(n)
    local colors = fill(false, n)
    local ancestors = collect(Int, 1:n)
    lcas = fill(-1, k)

    dfs_walk(tree) do v::Int
        if v >= 0   # v just finished
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
    lowest_common_ancestors(tree, parent, IncidenceIndex(length(tree), pairs))

