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
function lowest_common_ancestors(tree, pairs::NeighborIndex)::Vector{Int}
    k = length(pairs)
    return fill(-1, k)
end


lowest_common_ancestors(tree, pairs::Vector{Tuple{Int, Int}}) =
    lowest_common_ancestors(tree, NeighborIndex(length(tree), pairs))


end
