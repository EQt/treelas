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
    @assert parent[root_node(tree)] == root_node(tree)
    @show root_node(tree)
    push!(stack, ~root_node(tree))
    while !isempty(stack)
        v = pop!(stack)
        if v < 0
            @show ~v
            v = ~v
            push!(stack, v)
            for u in tree[v]
                push!(stack, ~u)
            end
        else
            @show v
            colors[v] = true

            if v != parent[v]
                unite!(uf, uf[v], uf[parent[v]])
                ancestors[uf[parent[v]]] = parent[v]
            end

            if v == 2
                @show "now!"
                @show Set(pairs[v])
            end
            for (u, ei) in pairs[v]
                @show u
                if colors[u]
                    @show (u, ei)
                    @show uf[u]
                    @show ancestors[uf[u]]
                    lcas[ei] = ancestors[uf[u]]
                end
            end
        end
    end

    return lcas
end


lowest_common_ancestors(tree, parent, pairs::Vector{Tuple{Int, Int}}) =
    lowest_common_ancestors(tree, parent, NeighborIndex(length(tree), pairs))


end
