"""
Bundle functions regarding (rooted) tree graphs.
"""
module Tree

include("tree/root.jl")

"""
A rooted tree represanted by its parent vector, i.e. if `u` is the parent node of `v` then `parent[v] == u`.
We always require that root is its own parent (`parent[root] == root`) and `length(parent) == n`.

Often you might want to have a [`ChildrenIndex`](@ref).
For debugging, the [`hierarchy`](@ref) function might be useful.
"""
struct RootedTree
    root::Int
    parent::Vector{Int}
end


RootedTree(parent::Vector{Int}) =
    RootedTree(find_root(parent), parent)


Base.length(t::RootedTree) =  length(t.parent)


include("tree/degrees.jl")
include("tree/children.jl")
include("tree/dfs.jl")
include("tree/lca.jl")
include("tree/inorder.jl")
include("tree/hierarchy.jl")

end
